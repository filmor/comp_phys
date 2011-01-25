#include <cstdlib>
#include <iostream>

#include "flat_position.hpp"
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
	typedef flat_position<dimensions> position_type;
	typedef sticky_particle<position_type> particle_type;
	typedef static_cluster<position_type> cluster_type;
	typedef diffusion_limited_bath<position_type, particle_type, cluster_type, size, 1> bath_type;
	typedef world<position_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;

	gl_visitor<position_type, particle_type, cluster_type, world_type, 1> glv(400, 400, false);
	population_visitor<position_type, particle_type, cluster_type, world_type, false, false, true> pv;

	for(unsigned int n = 0;; n++)
	{
		if(n % 1000000 == 0)
		{
			w.accept(glv);
			w.accept(pv);
		}
		w.step();
	}
}
