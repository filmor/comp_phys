#include <cstdlib>
#include <iostream>

#include "Topology.hpp"
#include "Particle.hpp"
#include "Cluster.hpp"
#include "Bath.hpp"
#include "World.hpp"
#include "population_visitor.hpp"
#include "gl_visitor.hpp"

using namespace trivial;

int main(int args, char ** argv)
{
	srand(time(0));

	const unsigned size = 800;
	const unsigned dimensions = 2;
	typedef FlatTopology<dimensions> topology_type;
	typedef StickyParticle<topology_type> particle_type;
	typedef StaticCluster<topology_type> cluster_type;
	typedef DiffusionLimitedBath<topology_type, particle_type, cluster_type, size, 1> bath_type;
	typedef World<topology_type, particle_type, cluster_type, bath_type> world_type;

	world_type w;

	GLVisitor<topology_type, particle_type, cluster_type, world_type> glv(400, 400, 1, false);
	PopulationVisitor<topology_type, particle_type, cluster_type, world_type, true> pv;

	for(unsigned int n = 0;; n++)
	{
		if(n % 10000 == 0)
		{
			w.accept(glv);
			w.accept(pv);
		}
		w.step();
	}
}
