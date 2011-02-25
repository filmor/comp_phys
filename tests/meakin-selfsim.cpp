#include <cstdlib>
#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
    srand(time(0));

    const unsigned dimensions = 2;
    typedef position<dimensions> position_type;
    typedef meakin::sticky_particle<position_type> particle_type;
    typedef meakin::moving_cluster<particle_type> cluster_type;
    typedef meakin::cluster_bath<particle_type, cluster_type, world<particle_type, meakin::static_cluster<particle_type>, 
            meakin::diffusion_limited_bath<particle_type, meakin::static_cluster<particle_type>, 1>>, 100>
        bath_type;
    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    for(unsigned int n = 0;; n++)
    {
        if(n % 1000 == 0)
        {
            w.accept(glv);
            w.accept(pv);
        }
        w.step();
    }
}
