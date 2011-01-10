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

	const unsigned size = 200;
	const unsigned dimensions = 2;
	const unsigned particles = size * size * 0.1;
	typedef FlatBoundedTopology<dimensions, size> topology_type;
	typedef StickyParticle<topology_type> particle_type;
	typedef MassiveStickyCluster<topology_type> cluster_type;
	typedef StaticBath<topology_type, particle_type, cluster_type, size, particles> bath_type;
	typedef World<topology_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;
	GLVisitor<topology_type, particle_type, cluster_type, world_type> glv(400, 400, 2, false);
	PopulationVisitor<topology_type, particle_type, cluster_type, world_type, true> pv;

	for(unsigned int n = 0;; n++)
	{
		w.accept(glv);
		if(n % 100 == 0)
			w.accept(pv);
		w.step();
	}
}
