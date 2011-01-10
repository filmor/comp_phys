#ifndef TRIVIAL_PARTICLE_HPP
#define TRIVIAL_PARTICLE_HPP

#include <vector>

#include "Topology.hpp"
#include "Cluster.hpp"
#include "Visitor.hpp"

namespace trivial
{
	//prototype. not implemented
	template<class TOPOLOGY>
	class Particle
	{
		public:
			static const char RESULT_NONE = 0, RESULT_CLUSTERED = 1, RESULT_NEW_CLUSTER = 2;
			typedef typename TOPOLOGY::position_type position_type;

			position_type position;	

			Particle(const position_type & pos) : position(pos) {};
			char interact(const std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters);

			void accept(Visitor<Particle<TOPOLOGY>> & visitor)
			{
				visitor.visit(this);
			}
	};

	template<class TOPOLOGY>
	class StickyParticle : public Particle<TOPOLOGY> // TODO sticky factor
	{
		public:
			typedef typename Particle<TOPOLOGY>::position_type position_type;
			typedef typename TOPOLOGY::neighbours_type neighbours_type;

			StickyParticle(const position_type & pos) : Particle<TOPOLOGY>(pos) {};

			char interact(const std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters)
			{
				const neighbours_type & neighbours = TOPOLOGY::neighbours(this->position);
				for(unsigned n = 0; n < TOPOLOGY::num_neighbours; ++n)
				{
					for(unsigned m = 0; m < clusters.size(); ++m)
						if(clusters[m]->has_particle_at(neighbours[n]))
						{
							clusters[m]->add_particle(this);
							return Particle<TOPOLOGY>::RESULT_CLUSTERED;
						}
					for(unsigned m = 0; m < particles.size(); ++m)
						if(TOPOLOGY::intersect(neighbours[n], particles[m]->position))
							return Particle<TOPOLOGY>::RESULT_NEW_CLUSTER; // other particle will be added in its interact or in the cluster's, i.e. within this step()
				}

				this->position = neighbours[rand() % TOPOLOGY::num_neighbours]; //TODO generator
				return Particle<TOPOLOGY>::RESULT_NONE;
			}
	};
}

#endif
