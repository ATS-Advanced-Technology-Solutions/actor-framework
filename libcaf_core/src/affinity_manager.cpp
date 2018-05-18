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

#ifdef CAF_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#include <sched.h>
#endif // CAF_LINUX
#ifdef CAF_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // CAF_WINDOWS

#include<set>

#include "caf/actor_system_config.hpp"
#include "caf/affinity/affinity_manager.hpp"

namespace caf {
namespace affinity {

manager::manager(actor_system& sys)
    : system_(sys) {
    // initialize atomics_ array
    for(auto& a: atomics_)  
	a.store(0);
}
    
void manager::start() {}
void manager::stop() {}

void manager::set_affinity(const actor_system::thread_type tt) {
    CAF_ASSERT(tt<actor_system::no_id);
    auto  cores  =cores_[tt];
    if (cores.size()) {
	auto& atomics =atomics_[tt];
	size_t id = atomics.fetch_add(1) % cores.size();
	auto Set{cores[id]};
	if (Set.size()) {

#if defined(CAF_LINUX) || defined(CAF_BSD)
	    // Create a cpu_set_t object representing a set of CPUs.
	    // Clear it and mark only CPU i as set.
	    cpu_set_t cpuset;
	    CPU_ZERO(&cpuset);
	    
	    std::for_each(Set.begin(), Set.end(), [&cpuset](int const& c) { 
		    //printf("Thread %p running on core %d\n", (void*)pthread_self(), c);
		    CPU_SET(c, &cpuset);
		});
	    
	    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset)<0) {
		//perror("sched_setaffinity");
		std::cerr << "Error setting affinity\n";		
	    }
#elif defined(CAF_WINDOWS)
	    // we do not conside the process affinity map 
	    DWORD_PTR mask = 0;
	    std::for_each(Set.begin(), Set.end(), [&cpuset](int const& c) { 
		    mask |= (static_cast<DWORD_PTR>(1) << c);
		});
	    auto ret = SetThreadAffinityMask(GetCurrentThread(),mask);
	    if (ret == 0) {
		std::cerr << "Error setting affinity\n";
	    }
#elif defined(CAF_MACOS)
	    std::cerr << "Thread affiity ignored in this platform\n";
#else
	    std::cerr << "Thread affiity not supported in this platform\n";
#endif    
	}
    }
}

std::string manager::getaffinityset(std::string& affinitystring) {
    size_t pos = affinitystring.find_first_of(SETSEPARATOR);
    if (pos == std::string::npos) {
	const std::string s{affinitystring};
	affinitystring.clear();
	return s;
    }
    const std::string s = affinitystring.substr(0,pos);
    affinitystring = affinitystring.substr(pos+1);
    return s;
}

std::set<int> manager::parseaffinityset(std::string s) {
    std::set<int> Set;
    const std::string scopy{s};
    const std::string SEPARATOR{RANGESEPARATOR+SPACE};
    auto checkhyphen = [](const std::string& s, int l, int r) {
	return (s.find_first_of("-",l,r-l) != std::string::npos);
    };
    
    try {
	while(!s.empty()) {
	    size_t pos1 = s.find_first_not_of(SPACE);
	    if (pos1 == std::string::npos) 
		s.clear();
	    else {
		size_t pos2 = s.find_first_of(SEPARATOR, pos1+1);
		if (pos2 == std::string::npos) {
		    int val = std::stoi(s);
		    if (val<0) throw std::invalid_argument("negative value");
		    Set.insert(val);
		    s.clear();
		} else {
		    size_t pos3 = s.find_first_not_of(SEPARATOR, pos2+1);
		    if ((pos3 != std::string::npos) && checkhyphen(s,pos1,pos3)) {
			int left=std::stoi(s.substr(pos1,pos2));
			if (left<0) throw std::invalid_argument("negative value");
			pos2 = s.find_first_of(SEPARATOR, pos3);
			int right = -1;
			if (pos2 == std::string::npos) {
			    right = std::stoi(s.substr(pos3));
			    if (right<0) throw std::invalid_argument("negative value");
			    s.clear();
			}
			else {
			    right = std::stoi(s.substr(pos3,pos2));
			    if (right<0) throw std::invalid_argument("negative value");
			    s = s.substr(pos2);
			}
			for(int k=left;k<=right;++k) Set.insert(k);
		    } else {
			if (checkhyphen(s,pos1, s.length())) throw std::invalid_argument("not a range");
			const std::string tmp=s.substr(pos1,pos2);
			int val = std::stoi(tmp);
			if (val<0) throw std::invalid_argument("negative value");
			Set.insert(val);
			s = s.substr(pos2);
		    }
		}
	    }
	}
    } catch (std::invalid_argument& e) {
	std::cerr << "Invalid argument into the set " << scopy << "\n";
	return std::set<int>();
    } catch (std::out_of_range& e) {
	std::cerr << "Number out of range into the set " << scopy << "\n";
	return std::set<int>();
    }    
    return Set;
}

void manager::init(actor_system_config& cfg) {
    worker_cores_   = cfg.affinity_worker_cores;
    detached_cores_ = cfg.affinity_detached_cores;
    blocking_cores_ = cfg.affinity_blocking_cores;
    other_cores_    = cfg.affinity_other_cores;

    auto Fill = [&](std::string copy, core_sets& c) {
	const size_t m = c.size();
	size_t i=0;
	while(!copy.empty()) {
	    std::set<int> Set = parseaffinityset(getaffinityset(copy));
	    if (m) c[i % m].insert(Set.begin(), Set.end());
	    else c.push_back(Set);
	    ++i;
	}
	return i;
    };

    auto num_workers_    = cfg.scheduler_max_threads;
    // workers
    auto& worker_cores = cores_[actor_system::worker_thread];
    worker_cores.resize(num_workers_);
    size_t howmany = Fill(worker_cores_, worker_cores);
    // fill out remaining (if any)
    if (howmany < num_workers_ && howmany>0 && worker_cores[howmany-1].size()) {
	for(size_t k=howmany, j=0; k<num_workers_; ++k, ++j) 
	    worker_cores[k]=worker_cores[j % howmany];	
    }
    // private (detached thread)
    auto& private_cores = cores_[actor_system::private_thread];
    Fill(detached_cores_, private_cores);
    // blocking
    auto& blocking_cores = cores_[actor_system::blocking_thread];
    Fill(blocking_cores_, blocking_cores);
    // other
    auto& other_cores = cores_[actor_system::other_thread];
    Fill(other_cores_, other_cores);
}
    
actor_system::module::id_t manager::id() const {
    return module::affinity_manager;
}

void* manager::subtype_ptr() {
    return this;
}

} // namespace affinity
} // namespace caf
