#include <cstdlib>
#include <iostream>

#include "periodic_position.hpp"
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

	const unsigned size = 800;
	const unsigned dimension = 2;
	typedef periodic_position<dimension, size> position_type;
	typedef sticky_particle<position_type> particle_type;
	typedef static_cluster<position_type> cluster_type;
	typedef uniform_bath<position_type, particle_type, cluster_type, size, 1> bath_type;
	typedef world<position_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;

	cluster_type * c = new cluster_type;
	position_type p;
	c->add_particle(new particle_type(p));
	p += position_type::unit_vectors[0];
	c->add_particle(new particle_type(p));
	w.get_clusters().push_back(c);

	gl_visitor<position_type, particle_type, cluster_type, world_type, 1> glv(400, 400, false);
	population_visitor<position_type, particle_type, cluster_type, world_type, true, false, false> pv;

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
