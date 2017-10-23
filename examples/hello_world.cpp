#include <string>
#include <iostream>

#include "caf/all.hpp"

using std::cout;
using std::endl;
using std::string;

using namespace caf;


int main() {
  config_value v1 = int64_t{42};
  config_value v2 = atom("hello");
  auto msg = make_message(v1, v2);
  cout << "v1: " << deep_to_string(v1) << endl;
  cout << "v2: " << deep_to_string(v2) << endl;
  cout << "msg: " << deep_to_string(msg) << endl;
}
