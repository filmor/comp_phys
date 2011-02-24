#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main()
{
    const unsigned size = 0;
    const unsigned dimensions = 2;
    
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

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    unsigned f = 0;
    for(unsigned int n = 0;; n++)
    {
        if(n % 1000000 == 0)
        {
            w.accept(glv);
            w.accept(pv);
            ++f;
            if (f > 50)
                break;
        }
        w.step();
    }
}
