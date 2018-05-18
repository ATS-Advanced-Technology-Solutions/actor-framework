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

#ifndef CAF_AFFINITY_MANAGER_HPP
#define CAF_AFFINITY_MANAGER_HPP

#include <string>
#include <vector>
#include <set>
#include <atomic>

#include "caf/actor_system.hpp"

namespace caf {
namespace affinity {

class manager: public actor_system::module {
public:
    const char SETSEPARATOR=',';
    const char RANGESEPARATOR='-';
    const std::string SPACE{" \n\r\t"};
    
    using core_sets    = std::vector<std::set<int>>;
    using core_array   = std::array<core_sets, actor_system::no_id>;
    using atomic_array = std::array<std::atomic<size_t>, actor_system::no_id>;

    explicit manager(actor_system& sys);

    inline std::string worker_cores() const {
	return worker_cores_;
    }

    inline std::string detached_cores() const {
	return detached_cores_;
    }

    inline std::string blocking_cores() const {
	return blocking_cores_;
    }

    inline std::string other_cores() const {
	return other_cores_;
    }
    
    void set_affinity(actor_system::thread_type tt); 

    void start() override;
    
    void init(actor_system_config& cfg) override;
    
    id_t id() const override;
    
    void* subtype_ptr() override;
    
    
protected:

    void stop() override;

    std::set<int> parseaffinityset(std::string s);

    std::string getaffinityset(std::string& affinitystring);


    std::string worker_cores_;
    std::string detached_cores_;
    std::string blocking_cores_;
    std::string other_cores_;

    core_array   cores_;
    atomic_array atomics_;

    actor_system& system_;   
};

} // namespace affinity 
} // namespace caf


#endif // CAF_AFFINITY_MANAGER_HPP
