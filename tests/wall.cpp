#include <random>

#include "position.hpp"
#include "tube.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main()
{
    const unsigned dimensions = 2;
    const int radius = 200;
    const int length = 200;
    const unsigned rate = 5;
    const unsigned slowness = 2;
    const unsigned spacing = 50;
   /* 
    const unsigned dimensions = 3;
    const int radius = 10;
    const int length = 20;
    const unsigned rate = 5;
    const unsigned slowness = 2;
*/
    typedef position<dimensions>
        position_type;
    
    typedef tube::flowing_particle<position_type, slowness>
        particle_type;
    
    typedef tube::flowing_cluster<particle_type>
        cluster_type;
    
    typedef tube::uniform_updater<particle_type, cluster_type, -length / 2, length / 2, radius, rate, spacing>
        updater_type;
    
    typedef world<particle_type, cluster_type, updater_type>
        world_type;

    world_type w;

    cluster_type bounds;
    position_type p;

    if(dimensions == 2)
        for(int y = 0; y <= 1.5 * radius; ++y)
        {
            p[0] = length / 2; p[1] = y;    bounds.add_particle(particle_type(p));
            p[1] = -y;                  bounds.add_particle(particle_type(p));
        }
    else
        for(p[0] = -length / 2; p[0] <= length / 2; ++p[0])
        {
            for(p[1] = -1.5 * radius; p[1] <= 1.5 * radius; ++p[1])
                for(p[2] = -1.5 * radius; p[2] <= 1.5 * radius; p[2] += 3 * radius)
                    bounds.add_particle(particle_type(p));
            for(p[2] = -1.5 * radius + 1; p[2] <= 1.5 * radius - 1; ++p[2])
                for(p[1] = -1.5 * radius; p[1] <= 1.5 * radius; p[1] += 3 * radius)
                    bounds.add_particle(particle_type(p));
        }

    w.add_cluster(bounds);

    gl_visitor<world_type> glv;

    for(unsigned int n = 0;; n++)
    {
        w.accept(glv);
        w.step();
    }
}
