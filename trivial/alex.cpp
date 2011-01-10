#include <cstdlib>
#include <iostream>

#include "Topology.hpp"
#include "Particle.hpp"
#include "Cluster.hpp"
#include "Bath.hpp"
#include "World.hpp"
#include "Visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
	srand(time(0));

	const unsigned size = 800;
	const unsigned dimensions = 2;
	typedef FlatBoundedTopology<dimensions, size> topology_type;
	typedef StickyParticle<topology_type> particle_type;
	typedef StaticCluster<topology_type> cluster_type;
	typedef UniformBath<topology_type, particle_type, cluster_type, size, 1> bath_type;
	typedef World<topology_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;

	cluster_type * c = new cluster_type;
	topology_type::position_type p;
	p[0] = p[1] = size / 2;
	c->add_particle(new particle_type(p));
	++p[1];
	c->add_particle(new particle_type(p));
	w.get_clusters().push_back(c);

	GLVisitor<topology_type, particle_type, cluster_type, world_type> glv(400, 400, 1, false);
	PopulationVisitor<topology_type, particle_type, cluster_type, world_type, true> pv;

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
