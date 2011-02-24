#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "statistics_visitor.hpp"

using namespace trivial;

int main()
{
    const unsigned size = 0;
    const unsigned dimensions = 7;

    typedef position<dimensions>
        position_type;

    typedef meakin::sticky_particle<position_type>
        particle_type;

    typedef meakin::static_cluster<particle_type>
        cluster_type;

    typedef meakin::diffusion_limited_updater<particle_type, cluster_type, size, 1>
        updater_type;

    typedef world<particle_type, cluster_type, updater_type>
        world_type;

    world_type w;

    statistics_visitor<world_type,
            stat::particles<world_type>,
            stat::coord_number<world_type>,
            stat::radius_of_gyration<world_type, 95>>//,
//            stat::dens_dens_correlation<world_type>>
        sv(std::cout);

    unsigned f = 0;
    for (unsigned int n = 0;; n++)
    {
        w.step();
        if(n % 100000 == 0)
        {
//            w.accept(glv);
            w.accept(sv);
            ++f;
            if (f > 1000)
                break;
        }
    }
}
