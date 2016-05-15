/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "eventql/mapreduce/tasks/SaveToTableTask.h"
#include "eventql/mapreduce/MapReduceScheduler.h"
#include "eventql/core/FixedShardPartitioner.h"
#include "eventql/infra/sstable/sstablereader.h"

using namespace util;

namespace eventql {

SaveToTableTask::SaveToTableTask(
    const AnalyticsSession& session,
    const String& table_name,
    Vector<RefPtr<MapReduceTask>> sources,
    MapReduceShardList* shards,
    AnalyticsAuth* auth,
    TSDBService* tsdb) :
    session_(session),
    table_name_(table_name),
    sources_(sources),
    auth_(auth) {
  Vector<size_t> input;

  size_t nshard = 0;
  for (const auto& src : sources_) {
    for (auto idx : src->shards()) {
      auto shard = mkRef(new SaveToTableTaskShard());
      shard->task = this;
      shard->dependencies.emplace_back(idx);
      shard->partition = FixedShardPartitioner::partitionKeyFor(
          table_name,
          nshard++);

      addShard(shard.get(), shards);
    }
  }
}

Option<MapReduceShardResult> SaveToTableTask::execute(
    RefPtr<MapReduceTaskShard> shard_base,
    RefPtr<MapReduceScheduler> job) {
  auto shard = shard_base.asInstanceOf<SaveToTableTaskShard>();
  const auto& deps = shard->dependencies;
  if (deps.size() != 1) {
    RAISE(kIllegalStateError, "SaveToTableTask must have exactly one source");
  }

  auto host = job->getResultHost(deps[0]);
  auto result_id = job->getResultID(deps[0]);
  if (host.isEmpty() || result_id.isEmpty()) {
    // FIXME save empty shard
    return None<MapReduceShardResult>();
  }

  logDebug(
      "z1.mapreduce",
      "Saving result shard to table; result_id=$4 target=$0/$1/$2/$3",
      host.get().addr.hostAndPort(),
      session_.customer(),
      table_name_,
      shard->partition.toString(),
      result_id.get().toString());

  auto url = StringUtil::format(
      "http://$0/api/v1/mapreduce/tasks/save_to_table",
      host.get().addr.ipAndPort());

  String params = StringUtil::format(
      "result_id=$0&partition=$1&table_name=$2",
      result_id.get().toString(),
      shard->partition.toString(),
      table_name_);

  auto api_token = auth_->encodeAuthToken(session_);

  http::HTTPMessage::HeaderList auth_headers;
  auth_headers.emplace_back(
      "Authorization",
      StringUtil::format("Token $0", api_token));

  http::HTTPClient http_client(&z1stats()->http_client_stats);
  auto req = http::HTTPRequest::mkPost(url, params, auth_headers);
  auto res = http_client.executeRequest(req);

  if (res.statusCode() == 204) {
    return None<MapReduceShardResult>();
  }

  if (res.statusCode() != 201) {
    RAISEF(
        kRuntimeError,
        "received non-201 response: $0", res.body().toString());
  }

  return None<MapReduceShardResult>();
}

} // namespace eventql
