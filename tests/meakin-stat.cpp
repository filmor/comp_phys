#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "statistics_visitor.hpp"

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

    typedef meakin::diffusion_limited_bath<particle_type, cluster_type, size, 1>
        bath_type;

    typedef world<particle_type, cluster_type, bath_type>
        world_type;

    world_type w;

    gl_visitor<world_type> glv;
    rog_visitor<world_type> rogv;

    unsigned f = 0;
    for(unsigned int n = 0;; n++)
    {
        if(n % 100000 == 0)
        {
            w.accept(glv);
            w.accept(rogv);
            ++f;
            if (f > 1000)
                break;
        }
        w.step();
    }
}
