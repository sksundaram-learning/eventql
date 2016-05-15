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
#ifndef _CM_ANALYTICSAPP_H
#define _CM_ANALYTICSAPP_H
#include "eventql/util/protobuf/MessageSchema.h"
#include "eventql/core/TSDBClient.h"
#include "eventql/core/TSDBService.h"
#include "eventql/core/CompactionWorker.h"
#include "eventql/api/LogfileService.h"
#include "eventql/api/EventsService.h"
#include "eventql/api/MapReduceService.h"
#include "eventql/AnalyticsSession.pb.h"
#include "eventql/ConfigDirectory.h"
#include <jsapi.h>

using namespace util;

namespace eventql {

class AnalyticsApp : public RefCounted {
public:

  AnalyticsApp(
      eventql::TSDBService* tsdb_node,
      eventql::PartitionMap* partition_map,
      eventql::ReplicationScheme* replication_scheme,
      eventql::CompactionWorker* cstable_index,
      ConfigDirectory* cdb,
      AnalyticsAuth* auth,
      csql::Runtime* sql,
      JSRuntime* js_runtime,
      const String& datadir,
      const String& cachedir);

  RefPtr<csql::TableProvider> getTableProvider(const String& customer) const;
  eventql::TSDBService* getTSDBNode() const;

  void insertMetric(
      const String& customer,
      const String& metric,
      const UnixTime& time,
      const String& value);

  // FIXPAUL remove
  void createTable(const TableDefinition& tbl);
  void updateTable(const TableDefinition& tbl, bool force = false);

  LogfileService* logfileService();
  EventsService* eventsService();
  MapReduceService* mapreduceService();

protected:

  void configureCustomer(const CustomerConfig& customer);
  void configureTable(const TableDefinition& tbl);

  eventql::TSDBService* tsdb_node_;
  eventql::PartitionMap* partition_map_;
  eventql::ReplicationScheme* replication_scheme_;
  eventql::CompactionWorker* cstable_index_;
  ConfigDirectory* cdb_;
  AnalyticsAuth* auth_;
  csql::Runtime* sql_;
  String datadir_;

  LogfileService logfile_service_;
  EventsService events_service_;
  MapReduceService mapreduce_service_;
};

eventql::TableDefinition tableDefinitionToTableConfig(const TableDefinition& tbl);

} // namespace eventql

#endif