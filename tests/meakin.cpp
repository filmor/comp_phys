#include <cstdlib>
#include <random>

#include "position.hpp"
#include "particle.hpp"
#include "cluster.hpp"
#include "bath.hpp"
#include "world.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
	srand(time(0));

	const unsigned size = 0;
	const unsigned dimensions = 2;
	typedef position<dimensions> position_type;
	typedef sticky_particle<position_type> particle_type;
	typedef diffusion_limited_bath<particle_type, size, 1> bath_type;
	typedef world<particle_type, bath_type, std::minstd_rand> world_type;

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
            if (f > 12)
                break;
		}
		w.step();
	}
}
