#include <cstdlib>
#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"
#include "statistics_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
    srand(time(0));

    const unsigned dimensions = 2;
    typedef position<dimensions> position_type;
    typedef meakin::sticky_particle<position_type> particle_type;
    typedef meakin::static_cluster<particle_type> cluster_type;
    typedef meakin::diffusion_limited_bath<particle_type, cluster_type, 1>
        bath_type;
    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    statistics_visitor<world_type,
            stat::particles<world_type>,
            stat::coord_number<world_type>,
            stat::radius_of_gyration<world_type, 95>>//,
//            stat::dens_dens_correlation<world_type>>
        sv(std::cout);

    unsigned f = 0;
    for(unsigned int n = 0;; n++)
    {
        w.step();
        if(n % 100000 == 0)
        {
            w.accept(glv);
            w.accept(sv);
            ++f;
            if (f > 1000)
                break;
        }
    }
}
