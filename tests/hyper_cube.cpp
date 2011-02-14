#include "hyper_cube.hpp"
#include "print.hpp"
#include <boost/optional.hpp>

using namespace trivial;

int main ()
{
    typedef hyper_cube<boost::optional<int>, 3> cube_type;
    cube_type cube(3);
    cube_type::index_type v;
    v[0] = 0; v[1] = 3; v[2] = 0;
    cube[v] = 5;
    
    DEBUG_PRINT(cube[v].is_initialized());
    DEBUG_PRINT(*cube[v]);
    v[1] = 1;
    DEBUG_PRINT(cube[v].is_initialized());

    cube.grow_around(v, 32);

    v[1] = 3;
    DEBUG_PRINT(cube[v].is_initialized());
    v[1] = 0;
    DEBUG_PRINT(cube[v].is_initialized());
    DEBUG_PRINT(*cube[v]);
}
