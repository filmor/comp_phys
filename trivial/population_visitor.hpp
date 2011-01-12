#ifndef TRIVIAL_POPULATION_VISITOR_HPP
#define TRIVIAL_POPULATION_VISITOR_HPP

#include "visitor.hpp"

namespace trivial
{
	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class WORLD, bool WORLDONLY>
	class PopulationVisitor : public OmniVisitor<TOPOLOGY, WORLD>
	{

		void visit(Particle<TOPOLOGY> * particle)
		{
			if(WORLDONLY)
				return;

			cout << "particle [" << particle << "] at (";
			for(unsigned n = 0; n < TOPOLOGY::dimensions - 1; ++n)
				cout << particle->position[n] << ",";
			cout << particle->position[TOPOLOGY::dimensions - 1] << ")" << endl;
		}

		void visit(Cluster<TOPOLOGY> * cluster)
		{
			if(WORLDONLY)
				return;

			cout << "cluster [" << cluster << "] at (";
			for(unsigned n = 0; n < TOPOLOGY::dimensions - 1; ++n)
				cout << cluster->get_bounds().center[n] << ",";
			cout << cluster->get_bounds().center[TOPOLOGY::dimensions - 1] << ") radius: " << cluster->get_bounds().radius << ", particles: " << ((CLUSTER *)cluster)->get_particles().size() << endl;

			for(unsigned n = 0; n < cluster->get_particles().size(); ++n)
			{
				cout << "\t";
				visit(cluster->get_particles()[n]);
			}
		}

		void visit(WORLD * world)
		{
			unsigned sum = 0;
			for(unsigned n = 0; n < world->get_clusters().size(); ++n)
				sum += world->get_clusters()[n]->get_particles().size();
			cout << "Currently " << world->get_particles().size() << " free particle(s) and " << sum << " in  " << world->get_clusters().size() << " cluster(s)." << endl;
		}
	};
}

#endif
