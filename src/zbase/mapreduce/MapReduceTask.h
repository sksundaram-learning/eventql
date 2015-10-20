/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#pragma once
#include "stx/stdtypes.h"
#include "stx/autoref.h"
#include "stx/option.h"
#include "stx/json/json.h"
#include "zbase/core/ReplicationScheme.h"

using namespace stx;

namespace zbase {

class MapReduceTask;

struct MapReduceTaskShard {
  RefPtr<MapReduceTask> task;
  Vector<size_t> dependencies;
};

struct MapReduceShardResult {
  ReplicaRef host;
  SHA1Hash result_id;
};

using MapReduceShardList = Vector<MapReduceTaskShard>;

class MapReduceScheduler;

class MapReduceTask : public RefCounted {
public:

  virtual Vector<size_t> build(MapReduceShardList* shards) = 0;

  virtual MapReduceShardResult execute(
      const MapReduceTaskShard& shard,
      MapReduceScheduler* job) = 0;

};


} // namespace zbase
