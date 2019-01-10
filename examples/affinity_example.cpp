
#include <chrono>
#include <iostream>
#include <math.h>
#include <set>

#include "caf/all.hpp"

#if defined(CAF_LINUX)
#include <unistd.h>
#endif
#if defined(CAF_WINDOWS)
#include <windows.h>
#endif

using namespace caf;
using namespace std;

void waste_time(int seconds) {
  auto work = chrono::seconds(seconds);
  auto start = chrono::high_resolution_clock::now();
  do {
    sin(1);
  } while (chrono::high_resolution_clock::now() - start < work);
}

void actor1(event_based_actor* self, int seconds) {
  CAF_IGNORE_UNUSED(self);
  waste_time(seconds);
}

void actor2(blocking_actor* self, int seconds) {
  CAF_IGNORE_UNUSED(self);
  waste_time(seconds);
}

void caf_main(actor_system& system) {
  cout << "CAF_VERSION=" << CAF_VERSION << endl;
  auto& aff_manager = system.affinity_manager();

  // spawn event based
  actor event1 = system.spawn(actor1, 45);
  actor event2 = system.spawn(actor1, 45);

  // spawn blocking actors
  actor block1 = system.spawn(actor2, 45);
  actor block2 = system.spawn(actor2, 45);

  // spawn detached actors
  actor det1 = system.spawn<detached>(actor1, 30);
  actor det2 = system.spawn<detached>(actor1, 30);

  cout << "actors started" << endl;

#if defined(CAF_LINUX)
  sleep(15);
#elif defined(CAF_WINDOWS)
  Sleep(15000);
#endif

  // move detached actors
  set<int> cores{1};
  aff_manager.set_actor_affinity(det1, cores);
  aff_manager.set_actor_affinity(det2, cores);

  cout << "actors moved" << endl;
}
CAF_MAIN()
