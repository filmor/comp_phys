#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <algorithm>

#include "Topology.hpp"
#include "Particle.hpp"
#include "Visitor.hpp"
#include "HyperCube.hpp"

namespace trivial
{
	//prototype. not implemented
	template<class TOPOLOGY>
	class Cluster
	{
		public:
			static const char RESULT_NONE = 0, RESULT_EMPTY = 1;

			typedef typename TOPOLOGY::position_type position_type;
			typedef typename TOPOLOGY::bounds_type bounds_type;


			Cluster() : map_(3, bounds_.center) {}

			bool interact(std::vector<Particle<TOPOLOGY> *> & particles, const std::vector<Cluster<TOPOLOGY> *> & clusters);

			void add_particle(Particle<TOPOLOGY> * const p)
			{
				particles_.push_back(p);

				if(particles_.size() == 1)
				{
					TOPOLOGY::init_bounds(bounds_, p->position);
					map_ = HyperCube<bool, TOPOLOGY::dimensions>(3, p->position);
				}
				else TOPOLOGY::add_to_bounds(bounds_, p->position);

				map_(p->position) = true;
			}

			void remove_particle(Particle<TOPOLOGY> * const p)
			{
				std::find(particles_.begin(), particles_.end(), p).erase();

				if(particles_.size() > 1)
				{
					TOPOLOGY::init_bounds(bounds_, particles_[0]->position);
					for(unsigned n = 0; n < particles_.size(); ++n)
						TOPOLOGY::add_to_bounds(bounds_, particles_[n]->position);
				}

				map_(p->position) = false;
			}

			void clear_particles()
			{
				particles_.clear();
				map_.clear(); 
			}

			bool has_particle(const Particle<TOPOLOGY> * const p) const
			{
				return std::find(particles_.begin(), particles_.end(), p) != particles_.end();
			}

			bool has_particle_at(const position_type & p) 
			{
				if(!TOPOLOGY::intersect(bounds_, p))
					return false;
				return map_(p);
			}

			inline const std::vector<Particle<TOPOLOGY> *> & get_particles() const
			{
				return particles_;
			}

			inline const bounds_type & get_bounds() const
			{
				return bounds_;
			}

			void accept(Visitor<Cluster<TOPOLOGY>> & visitor)
			{
				visitor.visit(this);
			}

		protected:
			HyperCube<bool, TOPOLOGY::dimensions> map_;
			std::vector<Particle<TOPOLOGY> *> particles_;
			bounds_type bounds_;

	};

	template<class TOPOLOGY>
	class StaticCluster : public Cluster<TOPOLOGY>
	{
		public:
			char interact(std::vector<Particle<TOPOLOGY> *> & particles, const std::vector<Cluster<TOPOLOGY> *> & clusters)
			{
				return this->particles_.empty() ? Cluster<TOPOLOGY>::RESULT_EMPTY : Cluster<TOPOLOGY>::RESULT_NONE;
			}
	};

	template<class TOPOLOGY>
	class StickyCluster : public StaticCluster<TOPOLOGY> //TODO sticky factor
	{
		public:
			typedef typename StaticCluster<TOPOLOGY>::position_type position_type;
			typedef typename TOPOLOGY::neighbours_type neighbours_type;

			char interact(std::vector<Particle<TOPOLOGY> *> & particles, const std::vector<Cluster<TOPOLOGY> *> & clusters)
			{
				if(this->particles_.empty())
					return Cluster<TOPOLOGY>::RESULT_EMPTY;

				bool merge = false;

				for(unsigned n = 0; n < particles.size(); ++n)
				{
					const neighbours_type & neighbours = TOPOLOGY::neighbours(particles[n]->position);
					for(unsigned m = 0; m < TOPOLOGY::num_neighbours; ++m)
						if(has_particle_at(neighbours[m]))
						{
							add_particle(particles[n]);
							particles.erase(particles.begin() + n);
							--n;
							merge = true;
							break;
						}
				}

				for(unsigned n = 0; n < clusters.size(); ++n)
				{
					if(clusters[n] == this)
						continue;

					for(unsigned m = 0; m < clusters[n]->get_particles().size(); ++m)
					{
						const neighbours_type & neighbours = TOPOLOGY::neighbours(clusters[n]->get_particles()[m]->position);
						for(unsigned k = 0; k < TOPOLOGY::num_neighbours; ++k)
						{
							if(has_particle_at(neighbours[k]))
							{
								for(unsigned l = 0; l < clusters[n]->get_particles().size(); ++l)
									add_particle(clusters[n]->get_particles()[l]);
								clusters[n]->clear_particles();
								merge = true;
								goto done;
							}
						}
					}
					done:;
				}

				if(!merge) // merge and moving would require to recursively check for merges made possible by merges
				{
					const neighbours_type & neighbours = TOPOLOGY::neighbours(this->bounds_);
					const position_type & pos = neighbours[rand() % TOPOLOGY::num_neighbours]; //TODO better generator
					position_type shift;
					for(unsigned n = 0; n < TOPOLOGY::dimensions; ++n)
					{
						shift[n] = pos[n] - this->bounds_.center[n];
						for(unsigned m = 0; m < this->particles_.size(); ++m)
							this->particles_[m]->position[n] += shift[n];
						this->bounds_.min[n] += shift[n];
						this->bounds_.max[n] += shift[n];
						this->bounds_.center[n] = pos[n];
					}
					this->map_.shift_offset(shift);
				}

				return Cluster<TOPOLOGY>::RESULT_NONE;
			}
	};

	template<class TOPOLOGY>
	class MassiveStickyCluster : public StickyCluster<TOPOLOGY>
	{
		public:
			char interact(std::vector<Particle<TOPOLOGY> *> & particles, const std::vector<Cluster<TOPOLOGY> *> & clusters)
			{
				if(this->particles_.size() == 0 || (rand() % this->particles_.size()) != 0)
					return this->particles_.empty() ? Cluster<TOPOLOGY>::RESULT_EMPTY : Cluster<TOPOLOGY>::RESULT_NONE;
				else return StickyCluster<TOPOLOGY>::interact(particles, clusters);
			}
	};
}

#endif
