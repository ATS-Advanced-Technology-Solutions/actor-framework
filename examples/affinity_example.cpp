
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

void actor1(event_based_actor* self, int seconds) {
  CAF_IGNORE_UNUSED(self);
  auto work = chrono::seconds(seconds);
  auto start = chrono::high_resolution_clock::now();
  do {
    sin(1);
  } while (chrono::high_resolution_clock::now() - start < work);
}

void caf_main(actor_system& system) {
  cout << "CAF_VERSION=" << CAF_VERSION << endl;
  auto& aff_manager = system.affinity_manager();

  actor a1 = system.spawn<detached>(actor1, 30);
  actor a2 = system.spawn<detached>(actor1, 30);

  cout << "actors started" << endl;

#if defined(CAF_LINUX)
  sleep(15);
#elif defined(CAF_WINDOWS)
  Sleep(15000);
#endif

  set<int> cores{1};
  aff_manager.set_actor_affinity(a1, cores);
  aff_manager.set_actor_affinity(a2, cores);

  cout << "actors moved" << endl;
}
CAF_MAIN()
