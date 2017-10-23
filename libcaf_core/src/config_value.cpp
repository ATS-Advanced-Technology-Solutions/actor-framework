/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2017                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "caf/config_value.hpp"

namespace caf {

config_value::config_value() : type_(variant_npos) {
  // Never empty ...
  set(typename detail::tl_head<types>::type());
}

config_value::config_value(const config_value& other) : type_(variant_npos) {
  set(other);
}

config_value::config_value(config_value&& other) : type_(variant_npos) {
  set(std::move(other));
}

config_value::~config_value() {
  destroy_data();
}

void config_value::destroy_data() {
  if (type_ == variant_npos)
    return;
  detail::variant_data_destructor f;
  apply(f);
}

void config_value::set(const config_value& other) {
  variant_assign_helper<config_value> helper{*this};
  other.apply(helper);
}

void config_value::set(config_value& other) {
  set(const_cast<const config_value&>(other));
}

void config_value::set(config_value&& other) {
  variant_move_helper<config_value> helper{*this};
  other.apply(helper);
}

} // namespace caf

