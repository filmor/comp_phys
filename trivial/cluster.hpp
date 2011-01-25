#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <algorithm>

#include "position.hpp"
#include "particle.hpp"
#include "visitor.hpp"
#include "hyper_cube.hpp"

namespace trivial
{
	//abc
	template<typename Position>
	class cluster
	{
		public:
			static const char RESULT_NONE, RESULT_EMPTY;

			cluster() : map_(3) {}

			bool interact(std::vector<particle<Position> *> & particles, const std::vector<cluster<Position> *> & clusters);

			void add_particle(particle<Position> * const p)
			{
				particles_.push_back(p);

				if(particles_.size() == 1)
					init_bounds(p->position);
				else add_to_bounds(p->position);

				map_((p->position - map_origin_).to_int_array()) = true;
			}

			void remove_particle(particle<Position> * const p)
			{
				std::find(particles_.begin(), particles_.end(), p).erase();

				if(particles_.size() > 1) //TODO too complicated, improve if needed frequently
				{
					init_bounds(particles_[0]->position);
					for(unsigned n = 0; n < particles_.size(); ++n)
						add_to_bounds(particles_[n]->position);
				}

				map_((p->position - map_origin_).to_int_array()) = false;
			}

			void clear_particles()
			{
				particles_.clear();
				map_.clear(); 
			}

			bool has_particle(const particle<Position> * const p) const
			{
				return std::find(particles_.begin(), particles_.end(), p) != particles_.end();
			}

			bool has_particle_at(const Position & p) 
			{
				if(abs2(p - center_) > radius2_)
					return false;
				return map_((p - map_origin_).to_int_array());
			}

			inline const std::vector<particle<Position> *> & get_particles() const
			{
				return particles_;
			}

			inline const Position & get_center() const
			{
				return center_;
			}

			inline const float get_radius() const
			{
				return radius_;
			}

			void accept(visitor<cluster<Position>> & visitor)
			{
				visitor.visit(this);
			}

		protected:
			std::vector<particle<Position> *> particles_;
			Position center_, map_origin_;
			hyper_cube<bool, Position::dimension> map_;

		private:
			float radius_;
			float radius2_;

			void init_bounds(Position & p)
			{
				center_ = p;
				radius_ = radius2_ = 1;
				map_ = hyper_cube<bool, Position::dimension>(3);
				map_origin_ = center_;
				map_origin_.round();
			}

			void add_to_bounds(Position & p)
			{
				float d2 = abs2(p - center_);
				if(d2 > radius2_)
				{
					// construct new center and radius as to enclose the previous sphere as well as additional point p
					// keep map_origin_ to prevent rounding mistakes upon look up
					center_ += 0.5f * (1 - radius_ / std::sqrt(d2)) * (p - center_);

					Position old_origin = map_origin_;
					map_origin_ = center_;
					map_origin_.round();
					map_.shift_origin((map_origin_ - old_origin).to_int_array());

					radius2_ = abs2(p - center_);
					radius_ = std::sqrt(radius2_);
				}
			}
	};
	
	template<typename Position>
	const char cluster<Position>::RESULT_NONE = 0;

	template<typename Position>
	const char cluster<Position>::RESULT_EMPTY = 1;

	template<typename Position>
	class static_cluster : public cluster<Position>
	{
		public:
			char interact(std::vector<particle<Position> *> & particles, const std::vector<cluster<Position> *> & clusters)
			{
				return this->particles_.empty() ? cluster<Position>::RESULT_EMPTY : cluster<Position>::RESULT_NONE;
			}
	};

	template<typename Position>
	class sticky_cluster : public static_cluster<Position> //TODO sticky factor
	{
		public:
			char interact(std::vector<particle<Position> *> & particles, const std::vector<cluster<Position> *> & clusters)
			{
				if(this->particles_.empty())
					return cluster<Position>::RESULT_EMPTY;

				bool merge = false;

				for(unsigned n = 0; n < particles.size(); ++n)
					for(int m = 0; m < Position::dimension * 2; ++m)
					{
						Position p = particles[n]->position + (2 * (m % 2) - 1) * Position::unit_vectors[m / 2];
						if(has_particle_at(p))
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
						for(int k = 0; k < Position::dimension * 2; ++k)
						{
							Position p = clusters[n]->get_particles()[m]->position + (2 * (k % 2) - 1) * Position::unit_vectors[k / 2];
							if(has_particle_at(p))
							{
								for(unsigned l = 0; l < clusters[n]->get_particles().size(); ++l)
									add_particle(clusters[n]->get_particles()[l]);
								clusters[n]->clear_particles();
								merge = true;
								goto done;
							}
						}
					done:;
				}

				if(!merge) // merge and moving would require to recursively check for merges made possible by merges
				{
					Position shift = (rand() % 2 ? 1 : -1) * Position::unit_vectors[rand() % Position::dimension]; //TODO generator
					for(unsigned n = 0; n < this->particles_.size(); ++n)
						this->particles_[n]->position += shift;
					this->center_ += shift;
					this->map_origin_ += shift;
				}

				return cluster<Position>::RESULT_NONE;
			}
	};

	template<typename Position>
	class massive_sticky_cluster : public sticky_cluster<Position>
	{
		public:
			char interact(std::vector<particle<Position> *> & particles, const std::vector<cluster<Position> *> & clusters)
			{
				if(this->particles_.size() == 0 || (rand() % this->particles_.size()) != 0) //TODO generator
					return this->particles_.empty() ? cluster<Position>::RESULT_EMPTY : cluster<Position>::RESULT_NONE;
				else return sticky_cluster<Position>::interact(particles, clusters);
			}
	};
}

#endif
