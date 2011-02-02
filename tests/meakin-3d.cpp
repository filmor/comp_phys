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
	const unsigned dimensions = 3;
	typedef flat_position<dimensions> position_type;
	typedef sticky_particle<position_type> particle_type;
	typedef static_cluster<position_type> cluster_type;
	typedef diffusion_limited_bath<position_type, particle_type, cluster_type, size, 1> bath_type;
	typedef world<position_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;

	gl_visitor<position_type, particle_type, cluster_type, world_type> & glv = gl_visitor<position_type, particle_type, cluster_type, world_type>::visitor();
	population_visitor<position_type, particle_type, cluster_type, world_type, false, false, true> pv;

	((gl_visitor_3d<position_type, particle_type, cluster_type, world_type> &)glv).anaglyph = ANA_LR | ANA_RG | ANA_RB;  // typical red/cyan glasses

	for(unsigned int n = 0;; n++)
	{
		if(n % 1000000 == 0)
			w.accept(pv);
		if(n % 100000 == 0)  // better response, cpu dependent pseudo-solution TODO: proper timing for visitors
			w.accept(glv);
		w.step();
	}
}
