/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_PERFETTO_TRACE_PROCESSOR_TRACE_PROCESSOR_H_
#define INCLUDE_PERFETTO_TRACE_PROCESSOR_TRACE_PROCESSOR_H_

#include <functional>
#include <memory>

#include "perfetto/trace_processor/basic_types.h"

namespace perfetto {

namespace protos {
class RawQueryArgs;
class RawQueryResult;
}  // namespace protos

namespace trace_processor {

class TraceProcessorImpl;

// Coordinates the loading of traces from an arbitrary source and allows
// execution of SQL queries on the events in these traces.
class TraceProcessor {
 public:
  explicit TraceProcessor(const Config&);
  ~TraceProcessor();

  // The entry point to push trace data into the processor. The trace format
  // will be automatically discovered on the first push call. It is possible
  // to make queries between two pushes.
  // Returns true if parsing has been succeeding so far, false if some
  // unrecoverable error happened. If this happens, the TraceProcessor will
  // ignore the following Parse() requests and drop data on the floor.
  bool Parse(std::unique_ptr<uint8_t[]>, size_t);

  // When parsing a bounded file (as opposite to streaming from a device) this
  // function should be called when the last chunk of the file has been passed
  // into Parse(). This allows to flush the events queued in the ordering stage,
  // without having to wait for their time window to expire.
  void NotifyEndOfFile();

  // Executes a SQLite query on the loaded portion of the trace. |result| will
  // be invoked once after the result of the query is available.
  void ExecuteQuery(const protos::RawQueryArgs&,
                    std::function<void(const protos::RawQueryResult&)>);

  // Interrupts the current query. Typically used by Ctrl-C handler.
  void InterruptQuery();

 private:
  std::unique_ptr<TraceProcessorImpl> impl_;
};

// When set, logs SQLite actions on the console.
void EnableSQLiteVtableDebugging();

}  // namespace trace_processor
}  // namespace perfetto

#endif  // INCLUDE_PERFETTO_TRACE_PROCESSOR_TRACE_PROCESSOR_H_