/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#pragma once

#ifdef CAF_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#include <unistd.h>
#include <syscall.h>
#endif // CAF_LINUX

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "caf/fwd.hpp"
#include "caf/config.hpp"

#ifdef CAF_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // CAF_WINDOWS

namespace caf {
namespace detail {

class private_thread {
public:
  enum worker_state {
    active,
    shutdown_requested,
    await_resume_or_shutdown
  };

  explicit private_thread(scheduled_actor* self);

  void run();

  bool await_resume();

  void resume();

  void shutdown();

  static void exec(private_thread* this_ptr);

  void notify_self_destroyed();

  void await_self_destroyed();

  void start();

#if defined(CAF_LINUX) || defined(CAF_BSD)
  pid_t get_native_pid();
#elif defined(CAF_WINDOWS)
  HANDLE get_native_pid();
#endif

private:
  void set_native_pid();

  std::mutex mtx_;
  std::condition_variable cv_;
  std::atomic<bool> self_destroyed_;
  std::atomic<scheduled_actor*> self_;
  std::atomic<worker_state> state_;
  actor_system& system_;
  std::atomic<int> native_pid_;
#if defined(CAF_WINDOWS)
  HANDLE native_handler_;
#endif
};

} // namespace detail
} // namespace caf

