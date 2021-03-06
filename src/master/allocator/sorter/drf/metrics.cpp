// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "master/allocator/sorter/drf/metrics.hpp"

#include <process/defer.hpp>

#include <process/metrics/metrics.hpp>

#include <stout/foreach.hpp>
#include <stout/path.hpp>

#include "master/allocator/sorter/drf/sorter.hpp"

using std::string;

using process::UPID;
using process::defer;

using process::metrics::Gauge;

namespace mesos {
namespace internal {
namespace master {
namespace allocator {

Metrics::Metrics(
    const UPID& _context,
    DRFSorter& _sorter,
    const std::string& _prefix)
  : context(_context),
    sorter(&_sorter),
    prefix(_prefix) {}


Metrics::~Metrics()
{
  foreachvalue (const Gauge& gauge, dominantShares) {
    process::metrics::remove(gauge);
  }
}


void Metrics::add(const string& client)
{
  CHECK(!dominantShares.contains(client));

  Gauge gauge(
      path::join(prefix, client, "/shares/", "/dominant"),
      defer(context, [this, client]() {
        return sorter->calculateShare(client);
      }));

  dominantShares.put(client, gauge);
  process::metrics::add(gauge);
}


void Metrics::remove(const string& client)
{
  CHECK(dominantShares.contains(client));

  process::metrics::remove(dominantShares.at(client));
  dominantShares.erase(client);
}

} // namespace allocator {
} // namespace master {
} // namespace internal {
} // namespace mesos {
