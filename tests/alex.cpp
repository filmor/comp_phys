#include <cstdlib>

#include "position.hpp"
#include "particle.hpp"
#include "bath.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main()
{
    srand(time(0));

    const unsigned size = 800;
    const unsigned dimension = 2;
    typedef position<dimension> position_type;
    typedef sticky_particle<position_type> particle_type;
    typedef uniform_bath<particle_type, size, 1> bath_type;
    typedef world<particle_type, bath_type> world_type;

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
