#ifndef TRIVIAL_POPULATION_VISITOR_HPP
#define TRIVIAL_POPULATION_VISITOR_HPP

#include "visitor.hpp"

namespace trivial
{
	using std::cout;
	using std::endl;

	template<typename Position, class Particle, class Cluster, class World, bool VisitWorld, bool VisitParticle, bool VisitCluster>
	class population_visitor : public omni_visitor<Position, World>
	{
		void visit(particle<Position> * particle)
		{
			if(!VisitParticle)
				return;

			cout << "particle [" << particle << "] at " << particle->position << endl;
		}

		void visit(cluster<Position> * cluster)
		{
			if(!VisitCluster)
				return;

			cout << "cluster [" << cluster << "] at " << cluster->get_center() << " radius: " << cluster->get_radius() << ", particles: " << cluster->get_particles().size() << endl;
		}

		void visit(World * world)
		{
			if(!VisitWorld)
				return;

			unsigned sum = 0;
			for(unsigned n = 0; n < world->get_clusters().size(); ++n)
				sum += world->get_clusters()[n]->get_particles().size();
			cout << "Currently " << world->get_particles().size() << " free particle(s) and " << sum << " in  " << world->get_clusters().size() << " cluster(s)." << endl;
		}
	};
}

#endif
