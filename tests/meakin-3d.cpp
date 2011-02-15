#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main()
{
    const unsigned size = 0;
    const unsigned dimensions = 3;

    typedef position<dimensions>
        position_type;

    typedef meakin::sticky_particle<position_type>
        particle_type;

    typedef meakin::static_cluster<particle_type>
        cluster_type;

    typedef meakin::diffusion_limited_bath<particle_type, cluster_type, size, 1>
        bath_type;
    
    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;
  
    // typical red/cyan glasses
    glv.set_anaglyph(ANA_RED, ANA_GREEN | ANA_BLUE);

    for(unsigned int n = 0;; n++)
    {
        if(n % 1000000 == 0)
            w.accept(pv);
        // better response, cpu dependent pseudo-solution
        // TODO: proper timing for visitors
        if(n % 100000 == 0)
            w.accept(glv);
        w.step();
    }
}
