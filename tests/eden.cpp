#include <cstdlib>
#include <random>

#include "position.hpp"
#include "eden.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
    const unsigned size = 0;
    const unsigned dimensions = 2;
    typedef position<dimensions> position_type;
    typedef eden::sticky_particle<position_type> particle_type;
    typedef eden::static_cluster<particle_type> cluster_type;
    typedef meakin::diffusion_limited_bath<particle_type, cluster_type, size, 1>
        bath_type;
    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    for(unsigned int n = 0;; n++)
    {
        w.step();
        if(n % 1000000 == 0)
        {
            w.accept(pv);
            w.accept(glv);
        }
    }
}
