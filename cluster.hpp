#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <algorithm>
#include <boost/optional.hpp>

#include "particle.hpp"
#include "visitor.hpp"
#include "hyper_cube.hpp"

namespace trivial
{

	template <class Particle>
	class cluster
	{
    public:
        typedef Particle particle_type;
        typedef typename Particle::position_type position_type;
        typedef vector<position_type::dimension, float> float_vector;

        cluster() : data_(3), count_(0), size_(3)
        {}

        void add_particle(Particle const& p)
        {
            auto relative_position = p.position - origin_;
            auto difference2 = abs2(relative_position);
            
            /// TODO: hyper_cube indexable by vector<N> or see if multi_array
            ///       supports this directly (vector<N> is iterable!)
            if (difference2 > radius2_)
            {
                float_vector pos (p.position);
                // Construct new center_
                center_ += 0.5f * (1 - radius_ / std::sqrt(difference2))
                                * (relative_position - center_);

                for (unsigned i = 0; i < origin_.size(); ++i)
                    // TODO Round correctly!
                    origin_[i] += int(center_[i] + 0.5f);

                data_.grow_around(origin_);

                // Should work for now. But still, investigate!
                radius2_ = difference2;
                radius_ = std::sqrt(radius2_);
            }

            data_[relative_position] = p;
            particles_.push_back(p);
        }

        void merge (cluster const& other)
        {
            // TODO
        }

        bool has_particle_at(position_type const& p) const
        {
            return data_[p - origin_].is_initialized();
        }

        std::vector<particle_type> const& get_particles () const
        {
            return particles_;
        }

        inline position_type const& get_center() const
        {
            return origin_;
        }

        inline float get_radius() const
        {
            return radius_;
        }

        template <typename T>
        void accept (visitor<T>& visitor)
        {
            visitor.visit(*this);
        }
        
        template <typename T>
        void accept (const_visitor<T>& visitor) const
        {
            visitor.visit(*this);
        }

        void move () {}

        inline friend
        char interact (cluster const& lhs, cluster const& rhs)
        {
            return 0;
        }

    private:
        float_vector center_;
        position_type origin_;
        
        hyper_cube<boost::optional<Particle>, position_type::dimension>
            data_;

        std::vector<particle_type> particles_;

        float radius_, radius2_;
        std::size_t count_;
        std::size_t size_;
	};

    /*
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
						Position p = particles[n].position + (2 * (m % 2) - 1) * Position::unit_vectors[m / 2];
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
					if(clusters[n] == this || abs(clusters[n]->get_center() - this->center_) > clusters[n]->get_radius() + this->radius_)
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
	};*/
}

#endif
