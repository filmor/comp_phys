#include "periodic_position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "tga_visitor.hpp"

using namespace trivial;

int main(int argc, char ** args)
{
    const unsigned size = 400;
    const unsigned dimension = 2;
    const unsigned particles = size * size * 0.1;

    typedef periodic_position<dimension, size>
        position_type;

    typedef meakin::sticky_particle<position_type>
        particle_type;

    typedef meakin::moving_cluster<particle_type>
        cluster_type;

    typedef meakin::static_updater<particle_type, cluster_type, size, particles>
        updater_type;

    typedef world<particle_type, cluster_type, updater_type, std::minstd_rand>
        world_type;

    world_type w;
    gl_visitor<world_type> glv;

    do
    {
        w.accept(glv);
        w.step();
    }
    while(w.get_clusters().size() > 1 || w.get_particles().size() > 0);

    if(argc > 1)
    {
        tga_visitor<world_type> tgav(args[1]);
        w.accept(tgav);
    }
}
