#include <cstdlib>
#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "statistics_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
    const unsigned dimensions = 2;

    typedef position<dimensions>
        position_type;

    typedef meakin::sticky_particle<position_type>
        particle_type;

    typedef meakin::static_cluster<particle_type>
        cluster_type;

    typedef meakin::diffusion_limited_updater<particle_type, cluster_type, 1>
        updater_type;

    typedef world<particle_type, cluster_type, updater_type>
        world_type;

    gl_visitor<world_type> glv;
    statistics_visitor<world_type,
            stat::particles<world_type>,
            stat::coord_number<world_type>,
            stat::radius_of_gyration<world_type, 95>>
        sv(std::cout);

    for(unsigned int m = 0; m < 10; ++m)
    {
        world_type w;

        for(unsigned int n = 0;; n++)
        {
            w.step();
            if(w.get_clusters().back().get_size() > 10000)
            {
                w.accept(glv);
                w.accept(sv);
                break;
            }
        }
    }
}
