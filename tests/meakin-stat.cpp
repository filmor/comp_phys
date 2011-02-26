#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "statistics_visitor.hpp"
#include "tga_visitor.hpp"

using namespace trivial;

int main(int argc, char ** args)
{
    unsigned max_particles = argc > 1 ? atoi(args[1]) : 10000;
    if(max_particles <= 0)
        max_particles = 10000;

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

    world_type w;

    statistics_visitor<world_type,
            stat::particles<world_type>,
            stat::coord_number<world_type>,
            stat::radius_of_gyration<world_type, 95>,
            stat::radius_of_gyration<world_type, 90>,
            stat::radius_of_gyration<world_type, 75>,
            stat::radius_of_gyration<world_type, 50>>
        rogv(std::cout);

    statistics_visitor<world_type,
            stat::particles<world_type>,
            stat::density<world_type, 100>,
            stat::dens_dens_correlation<world_type>>
        totalv(std::cout);

    unsigned last_number = 0;
    do 
    {
        w.step();
        if(w.get_clusters().back().get_size() >= last_number + 100)
        {
            w.accept(rogv);
            last_number = w.get_clusters().back().get_size();
        }
    }
    while(last_number < max_particles);
    w.accept(totalv);

    if(argc >= 2)
    {
        tga_visitor<world_type> tgav(args[2]);
        w.accept(tgav);
    }
}
