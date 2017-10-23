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

#ifndef CAF_CONFIG_VALUE_HPP
#define CAF_CONFIG_VALUE_HPP

#include <string>
#include <vector>
#include <cstdint>

#include "caf/atom.hpp"
#include "caf/variant.hpp"
#include "caf/duration.hpp"

namespace caf {

/// A type for config parameters with similar interface to a `variant`. This
/// type is not implemented as a simple variant alias because variants cannot
/// contain lists of themselves.
class config_value {
public:
  using T0 = std::string;
  using T1 = double;
  using T2 = int64_t;
  using T3 = bool;
  using T4 = atom_value;
  using T5 = duration;
  using T6 = std::vector<config_value>;

  using types = detail::type_list<T0, T1, T2, T3, T4, T5, T6>;

  static constexpr size_t num_types = 7;

  static constexpr int max_type_id = 6;

  using type0 = std::string;

  config_value();

  config_value(const config_value& other);

  config_value(config_value&& other);

  template <class T>
  config_value(T&& x) : type_(variant_npos) {
    set(std::forward<T>(x));
  }

  template <class T>
  config_value& operator=(T&& x) {
    set(std::forward<T>(x));
    return *this;
  }

  ~config_value();

  inline size_t index() const {
    return type_;
  }

  inline bool valueless_by_exception() const {
    return type_ == variant_npos;
  }

  /// @cond PRIVATE
  template <int Pos>
  bool is(std::integral_constant<int, Pos>) const {
    return type_ == Pos;
  }

  template <class T>
  bool is() const {
    using namespace detail;
    int_token<tl_index_where<types,
                             tbind<is_same_ish, T>::template type>::value>
      token;
    return is(token);
  }

  template <class Visitor>
  auto apply(Visitor&& visitor) const
  -> decltype(visitor(std::declval<const type0&>())) {
    return variant_apply_impl(*this, std::forward<Visitor>(visitor));
  }

  template <class Visitor>
  auto apply(Visitor&& visitor) -> decltype(visitor(std::declval<type0&>())) {
    return variant_apply_impl(*this, std::forward<Visitor>(visitor));
  }

  CAF_VARIANT_DATA_GETTER(0)
  CAF_VARIANT_DATA_GETTER(1)
  CAF_VARIANT_DATA_GETTER(2)
  CAF_VARIANT_DATA_GETTER(3)
  CAF_VARIANT_DATA_GETTER(4)
  CAF_VARIANT_DATA_GETTER(5)
  CAF_VARIANT_DATA_GETTER(6)

  /// @endcond

private:
  void destroy_data();

  template <class T>
  void set(T&& x) {
    using type = typename std::decay<T>::type;
    static constexpr int type_id = detail::tl_index_where<
      types, detail::tbind<is_same_ish, type>::template type>::value;
    static_assert(type_id >= 0, "invalid type for variant");
    std::integral_constant<int, type_id> token;
    if (type_ != type_id) {
      destroy_data();
      type_ = type_id;
      auto& ref = get(token);
      new (&ref) type(std::forward<T>(x));
    } else {
      get(token) = std::forward<T>(x);
    }
  }

  void set(const config_value& other);

  void set(config_value& other);

  void set(config_value&& other);

  size_t type_;

  union {
    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
    T6 v6;
  };
};

template <>
struct is_variant<config_value> : std::true_type {};

/// @relates config_value
template <class Inspector>
typename std::enable_if<Inspector::reads_state,
                        typename Inspector::result_type>::type
inspect(Inspector& f, config_value& x) {
  auto type_tag = x.index();
  variant_reader<config_value> helper{type_tag, x};
  return f(meta::omittable(), type_tag, helper);
}

/// @relates config_value
template <class Inspector>
typename std::enable_if<Inspector::writes_state,
                        typename Inspector::result_type>::type
inspect(Inspector& f, config_value& x) {
  size_t type_tag;
  variant_writer<config_value> helper{type_tag, x};
  return f(meta::omittable(), type_tag, helper);
}

} // namespace caf

#endif // CAF_CONFIG_VALUE_HPP
