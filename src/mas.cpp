#include <iostream>

#include "cppa/cppa.hpp"

#include "mas/agent.hpp"
#include "mas/environment.hpp"

using namespace std;
using namespace cppa;


auto main () -> int{

    auto env = spawn<environment>();


    await_all_others_done();
    shutdown();
    return EXIT_SUCCESS;
}
