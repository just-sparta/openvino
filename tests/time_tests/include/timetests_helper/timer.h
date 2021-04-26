// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <chrono>
#include <string>

namespace TimeTest {
using time_point = std::chrono::high_resolution_clock::time_point;

/** Encapsulate time measurements.
Object of a class measures time at start and finish of object's life cycle.
When destroyed, reports duration.
*/
class Timer {
private:
  std::string name;
  int order;
  time_point start_time;

public:
  /// Constructs Timer object and measures start time.
  Timer(const std::string &timer_name, int order_number);

  /// Destructs Timer object, measures duration and reports it.
  ~Timer();
};

#define SCOPED_TIMER(timer_name, order_number) TimeTest::Timer timer_name(#timer_name, order_number);

} // namespace TimeTest