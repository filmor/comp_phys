#ifndef TRIVIAL_POPULATION_VISITOR_HPP
#define TRIVIAL_POPULATION_VISITOR_HPP

#include "visitor.hpp"
#include "print.hpp"

namespace trivial
{
	using std::cout;
	using std::endl;

	template<class World, bool VisitWorld, bool VisitParticle, bool VisitCluster>
	class population_visitor : public const_world_visitor<World>
	{
		void visit(typename World::particle_type const& particle)
		{
			if(!VisitParticle)
				return;

			print ("particle at", particle.position);
		}

		void visit(typename World::cluster_type const& cluster)
		{
			if(!VisitCluster)
				return;

		    print ("cluster at", cluster.get_center(),
                   "radius:", cluster.get_radius(),
                   "particles:", cluster.get_particles().size()
                  );
		}

		void visit(World const& world)
		{
			if(!VisitWorld)
				return;

			unsigned sum = 0;
			for(unsigned n = 0; n < world.get_clusters().size(); ++n)
				sum += world.get_clusters()[n].get_particles().size();
				;

			print("Currently ", world.get_particles().size(),
                  "free particle(s) and", sum, "in",
                  world.get_clusters().size(), "cluster(s)."
                 );
		}
	};
}

#endif
