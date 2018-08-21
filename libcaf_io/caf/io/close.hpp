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

#ifndef CAF_IO_CLOSE_HPP
#define CAF_IO_CLOSE_HPP

#include <cstdint>

#include "caf/actor_system.hpp"

#include "caf/io/middleman.hpp"

namespace caf {
namespace io {

/// Closes port `port` regardless of whether an actor is published to it.
inline expected<void> close(actor_system& sys, uint16_t port) {
  return sys.middleman().close(port);
}

} // namespace io
} // namespace caf

#endif // CAF_IO_CLOSE_HPP
