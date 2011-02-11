#include <cstdlib>
#include <iostream>

#include "periodic_position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "population_visitor.hpp"

using namespace trivial;

int main()
{
    srand(time(0));

    const unsigned size = 400;
    const unsigned dimension = 2;
    const unsigned particles = size * size * 0.1;
    typedef periodic_position<dimension, size> position_type;
    typedef meakin::sticky_particle<position_type> particle_type;
    typedef meakin::moving_cluster<particle_type> cluster_type;
    typedef meakin::static_bath<particle_type, cluster_type, size, particles>
        bath_type;
    typedef world<particle_type, cluster_type, bath_type> world_type;

    world_type w;
    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    for (unsigned int n = 0;; ++n)
    {
        w.accept(glv);
        if(n % 100 == 0)
            w.accept(pv);
        w.step();
    }
}
