#include <cstdlib>
#include <random>

#include "position.hpp"
#include "eden.hpp"
#include "world.hpp"
#include "statistics_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
    const unsigned dimensions = 2;
    typedef position<dimensions> position_type;
    typedef eden::static_particle<position_type> particle_type;
    typedef eden::static_cluster<particle_type> cluster_type;
    typedef eden::nearest_neighbours_bath<particle_type, cluster_type>
        bath_type;
    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    statistics_visitor<world_type, stat::particles<world_type>, stat::score_dist<world_type, 5>, stat::radius_of_gyration<world_type, 100>> sv(std::cout);

    for(unsigned int n = 0;; n++)
    {
        w.step();
       // if(n % 100 == 0)
        {
        //    w.accept(sv);
            w.accept(glv);
        }
    }
}
