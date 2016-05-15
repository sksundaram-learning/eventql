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
#pragma once
#include <eventql/util/stdtypes.h>
#include <eventql/util/SHA1.h>
#include <eventql/CustomerConfig.h>
#include <eventql/core/ClusterConfig.pb.h>
#include <eventql/TableDefinition.h>

using namespace util;

namespace eventql {

class ConfigDirectoryMaster {
public:

  ConfigDirectoryMaster(const String& path);

  ClusterConfig fetchClusterConfig() const;
  ClusterConfig updateClusterConfig(ClusterConfig config);

  CustomerConfig fetchCustomerConfig(const String& customer_key) const;
  CustomerConfig updateCustomerConfig(CustomerConfig config);

  TableDefinition fetchTableDefinition(
      const String& customer_key,
      const String& table_key);
  TableDefinitionList fetchTableDefinitions(const String& customer_key);
  TableDefinition updateTableDefinition(
      TableDefinition table,
      bool force = false);

  UserDB fetchUserDB();
  UserConfig fetchUserConfig(
      const String& userid);
  UserConfig updateUserConfig(
      UserConfig table,
      bool force = false);

  Vector<Pair<String, uint64_t>> heads() const;

protected:

  void loadHeads();

  mutable std::mutex mutex_;
  String customerdb_path_;
  String userdb_path_;
  String clusterdb_path_;
  HashMap<String, uint64_t> heads_;
};

} // namespace eventql