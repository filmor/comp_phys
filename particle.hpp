#ifndef TRIVIAL_PARTICLE_HPP
#define TRIVIAL_PARTICLE_HPP

#include <vector>

#include "position.hpp"
#include "cluster.hpp"
#include "visitor.hpp"

namespace trivial
{
	//abc
	template<typename Position>
	class particle
	{
		public:
			static const char RESULT_NONE = 0, RESULT_CLUSTERED = 1, RESULT_NEW_CLUSTER = 2;

			Position position;	

			particle(const Position & pos) : position(pos) {};
			char interact(const std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters);

			void accept(visitor<particle<Position>> & visitor)
			{
				visitor.visit(this);
			}
	};

	template<typename Position>
	class sticky_particle : public particle<Position> // TODO sticky factor
	{
		public:
			sticky_particle(const Position & pos) : particle<Position>(pos) {};

			char interact(const std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters)
			{
				for(int n = 0; n < Position::dimension * 2; ++n)
				{
					Position p = this->position + (2 * (n % 2) - 1) * Position::unit_vectors[n / 2];
				
					for(unsigned m = 0; m < clusters.size(); ++m)
						if(clusters[m]->has_particle_at(p))
						{
							clusters[m]->add_particle(this);
							return particle<Position>::RESULT_CLUSTERED;
						}
					for(unsigned m = 0; m < particles.size(); ++m)
						if(p == particles[m]->position)
							return particle<Position>::RESULT_NEW_CLUSTER; // other particle will be added in its interact or in the cluster's, i.e. within this step()
				}

				this->position += (rand() % 2 ? 1 : -1) * Position::unit_vectors[rand() % Position::dimension]; //TODO generator
				return particle<Position>::RESULT_NONE;
			}
	};
}

#endif
