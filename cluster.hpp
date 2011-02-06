#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <algorithm>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "interaction.hpp"
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

        cluster() : data_(3), count_(0)
        {}

        void add_particle(Particle const& p)
        {
            auto relative_position = p.position - origin_;
            auto difference2 = abs2(relative_position);
            
            /// TODO: hyper_cube indexable by vector<N> or see if multi_array
            ///       supports this directly (vector<N> is iterable!)
            if (difference2 > radius2_)
            {
                typename position_type::float_vector_type pos (p.position);
                // Construct new offset_
/*                offset_ += 0.5f * (1 - radius_ / std::sqrt(difference2))
                                * (relative_position - offset_);

                for (unsigned i = 0; i < origin_.size(); ++i)
                    // TODO Round correctly!
                    origin_[i] += int(offset_[i] + 0.5f); */

                // Should work for now. But still, investigate!
                radius2_ = difference2;
                radius_ = std::sqrt(radius2_);

                if (radius_ >= data_.get_radius())
                    // TODO: It should grow around new_origin_ - origin_
                    //       and to the nearest power of 2 greater than
                    //       radius_
                    data_.grow();
            }

            data_[relative_position] = p;
            particles_.push_back(p);
        }

        void merge (cluster const& other)
        {
            BOOST_FOREACH(particle_type const& p, other.particles_)
            {
                add_particle(p);
            }
            // TODO: Remove particle from other
        }

        bool has_particle_at(position_type const& p) const
        {
            if (abs_1(p) <= data_.get_radius())
                return data_[p - origin_].is_initialized();
            else
                return false;
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

        void move (typename position_type::vector_type const& vec)
        {
            // origin_ += vec;
            // TODO: Don't move clusters for now, useful only in meakin2 where
            //       we still have to define sticky_clusters new semantic
        }

        inline friend
        interaction::result_type interact (cluster const& lhs, cluster const& rhs)
        {
            return interaction::NONE;
        }

    private:
        // typename position_type::float_vector_type offset_;
        position_type origin_;
        
        hyper_cube<boost::optional<Particle>, position_type::dimension>
            data_;

        std::vector<particle_type> particles_;

        float radius_, radius2_;
        std::size_t count_;
        std::size_t size_;
	};

    /*
	template <typename Particle>
	class sticky_cluster : public cluster<Position>
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
					if(clusters[n] == this || abs(clusters[n]->get_center() - this->offset_) > clusters[n]->get_radius() + this->radius_)
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
					this->offset_ += shift;
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
