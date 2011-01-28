#ifndef TRIVIAL_WOLRD_HPP
#define TRIVIAL_WORLD_HPP

#include <vector>

#include "position.hpp"
#include "particle.hpp"
#include "cluster.hpp"
#include "bath.hpp"
#include "visitor.hpp"

namespace trivial
{
	template<typename Position, class Particle, class Cluster, class Bath>
	class world
	{
		public:
			void step()
			{
				bath_.step(particles_, clusters_);
		
				for(unsigned n = 0; n < particles_.size(); ++n)
					switch(((Particle *)particles_[n])->interact(particles_, clusters_))
					{
						case particle<Position>::RESULT_NEW_CLUSTER:
							{
								cluster<Position> * c = new Cluster;
								c->add_particle(particles_[n]);
								clusters_.push_back(c);
							}
						case particle<Position>::RESULT_CLUSTERED:
							particles_.erase(particles_.begin() + n);
							--n;
							break;
					}

				for(unsigned n = 0; n < clusters_.size(); ++n)
					switch(((Cluster *)clusters_[n])->interact(particles_, clusters_))
					{
						case cluster<Position>::RESULT_EMPTY:
							delete clusters_[n];
							clusters_.erase(clusters_.begin() + n);
							--n;
							break;
					}
			}

			void accept(omni_visitor<Position, world> & v)
			{
				for(unsigned n = 0; n < particles_.size(); ++n)
					particles_[n]->accept(v);
				for(unsigned n = 0; n < clusters_.size(); ++n)
					clusters_[n]->accept(v);
				((visitor<world> &)v).visit(this);
			}

			~world()
			{
				for(unsigned n = 0; n < particles_.size(); ++n)
					delete particles_[n];
				for(unsigned n = 0; n < clusters_.size(); ++n)
					delete clusters_[n];
			}

			inline const std::vector<particle<Position> *> & get_particles() const
			{
				return particles_;
			}
	
			inline /*const*/ std::vector<cluster<Position> *> & get_clusters() /*const*/ //TODO write seeding visitor instead
			{
				return clusters_;
			}

		private:
			std::vector<particle<Position> *> particles_;
			std::vector<cluster<Position> *> clusters_;
			Bath bath_;
	};
}

#endif

