#ifndef TRIVIAL_WOLRD_HPP
#define TRIVIAL_WORLD_HPP

#include <vector>

#include "Topology.hpp"
#include "Particle.hpp"
#include "Cluster.hpp"
#include "Bath.hpp"
#include "Visitor.hpp"

namespace trivial
{
	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class BATH>
	class World
	{
		public:
			void step()
			{
				bath_.step(particles_, clusters_);
		
				for(unsigned n = 0; n < particles_.size(); ++n)
					switch(((PARTICLE *)particles_[n])->interact(particles_, clusters_))
					{
						case Particle<TOPOLOGY>::RESULT_NEW_CLUSTER:
							{
								Cluster<TOPOLOGY> * c = new CLUSTER;
								c->add_particle(particles_[n]);
								clusters_.push_back(c);
							}
						case Particle<TOPOLOGY>::RESULT_CLUSTERED:
							particles_.erase(particles_.begin() + n);
							--n;
							break;
					}

				for(unsigned n = 0; n < clusters_.size(); ++n)
					switch(((CLUSTER *)clusters_[n])->interact(particles_, clusters_))
					{
						case Cluster<TOPOLOGY>::RESULT_EMPTY:
							delete clusters_[n];
							clusters_.erase(clusters_.begin() + n);
							--n;
							break;
					}
			}

			void accept(OmniVisitor<TOPOLOGY, World<TOPOLOGY, PARTICLE, CLUSTER, BATH>> & visitor)
			{
				for(unsigned n = 0; n < particles_.size(); ++n)
					particles_[n]->accept((Visitor<Particle<TOPOLOGY>> &)visitor);
				for(unsigned n = 0; n < clusters_.size(); ++n)
					clusters_[n]->accept((Visitor<Cluster<TOPOLOGY>> &)visitor);
				((Visitor<World<TOPOLOGY, PARTICLE, CLUSTER, BATH>> &)visitor).visit(this);
			}

			~World()
			{
				for(unsigned n = 0; n < particles_.size(); ++n)
					delete particles_[n];
				for(unsigned n = 0; n < clusters_.size(); ++n)
					delete clusters_[n];
			}

			inline const std::vector<Particle<TOPOLOGY> *> & get_particles() const
			{
				return particles_;
			}
	
			inline /*const*/ std::vector<Cluster<TOPOLOGY> *> & get_clusters() /*const*/ // temporarily not return const to be able to prepare clusters in main()
			{
				return clusters_;
			}

		private:
			std::vector<Particle<TOPOLOGY> *> particles_;
			std::vector<Cluster<TOPOLOGY> *> clusters_;
			BATH bath_;

	};
}

#endif

