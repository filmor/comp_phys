#include <random>

#include "position.hpp"
#include "meakin.hpp"
#include "eden.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main()
{
    const unsigned dimensions = 2;
    
    typedef position<dimensions>
        position_type;
    
    typedef meakin::sticky_particle<position_type>
        particle_type;
    
    typedef meakin::static_cluster<particle_type>
        cluster_type;
    
    typedef eden::nearest_neighbours_updater<particle_type, cluster_type>
        updater_type;
    
    typedef world<particle_type, cluster_type, updater_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    for(unsigned int n = 0;; n++)
    {
        if(n % 100 == 0)
        {
            w.accept(glv);
            w.accept(pv);
        }
        w.step();
    }
}
