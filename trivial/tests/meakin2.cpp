#include <cstdlib>
#include <iostream>

#include "periodic_position.hpp"
#include "particle.hpp"
#include "cluster.hpp"
#include "bath.hpp"
#include "world.hpp"
#include "gl_visitor.hpp"
#include "population_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
	srand(time(0));

	const unsigned size = 200;
	const unsigned dimension = 2;
	const unsigned particles = size * size * 0.1;
	typedef periodic_position<dimension, size> position_type;
	typedef sticky_particle<position_type> particle_type;
	typedef sticky_cluster<position_type> cluster_type;
	typedef static_bath<position_type, particle_type, cluster_type, size, particles> bath_type;
	typedef world<position_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;
	gl_visitor<position_type, particle_type, cluster_type, world_type, 2> glv(400, 400, false);
	population_visitor<position_type, particle_type, cluster_type, world_type, true, false, false> pv;

	for(unsigned int n = 0;; n++)
	{
		w.accept(glv);
		if(n % 100 == 0)
			w.accept(pv);
		w.step();
	}
}
