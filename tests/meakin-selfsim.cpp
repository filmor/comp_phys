#include <cstdlib>
#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "tga_visitor.hpp"

#ifndef FACTOR          //sensible between 10 and 30
#define FACTOR 10
#endif

using namespace trivial;

int main(int argc, char ** args)
{
    const unsigned dimensions = 2;

    typedef position<dimensions>
        position_type;

    typedef meakin::sticky_particle<position_type>
        particle_type;

    typedef meakin::moving_cluster<particle_type>
        cluster_type;

    typedef meakin::cluster_updater<particle_type, cluster_type, 
            world<particle_type, cluster_type, meakin::cluster_updater<particle_type, cluster_type,
                world<particle_type, cluster_type, meakin::cluster_updater<particle_type, cluster_type,
                    world<particle_type, cluster_type, meakin::diffusion_limited_updater<particle_type, cluster_type, 1>>,
                FACTOR>>,
            FACTOR * FACTOR>>,
        FACTOR * FACTOR * FACTOR>
        bath_type;

    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;
    do 
    { w.step(); }
    while(w.get_clusters().back().get_size() < 10000);

    if(argc >= 1)
    {
        tga_visitor<world_type> tgav(args[1]);
        w.accept(tgav);
    }
}
