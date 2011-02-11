#include <cstdlib>

#include "position.hpp"
#include "meakin.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main ()
{
    srand(time(0));

    const unsigned size = 800;
    const unsigned dimension = 2;
    typedef position<dimension> position_type;
    typedef meakin::sticky_particle<position_type> particle_type;
    typedef meakin::static_cluster<particle_type> cluster_type;
    typedef meakin::uniform_bath<particle_type, cluster_type, size, 1> bath_type;
    typedef world<particle_type, cluster_type, bath_type> world_type;

    world_type w;

    world_type::cluster_type c;
    position_type p;
    c.add_particle(particle_type(p));
    p += get_unit_vector<position_type>(0);
    c.add_particle(particle_type(p));
    w.add_cluster(c);

    gl_visitor<world_type> glv;
    population_visitor<world_type> pv;

    for(unsigned int n = 0;; n++)
    {
        if(n % (unsigned)1E7 == 0)
        {
            w.accept(glv);
            w.accept(pv);
        }
        w.step();
    }
}
