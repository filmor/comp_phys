#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "print.hpp"
#include "interaction.hpp"
#include "hyper_cube.hpp"

namespace trivial
{
namespace meakin
{

    template <class Particle>
    class static_cluster
    {
    public:
        typedef Particle particle_type;
        typedef typename Particle::position_type position_type;

        static_cluster () : data_(3)
        {}

        void add_particle(Particle p)
        {
            p.position -= origin_;
            auto difference2 = abs2(p.position);
            
            if (difference2 > radius2_)
            {
                auto new_radius = std::sqrt(difference2);

                // Construct new offset_:
                // offset_ has to move along relative_position
                // by 1/2 * (1 - radius / new_radius)
                offset_ += 0.5f * (radius_ / new_radius)
                                * (p.position - offset_);
                // print("offset", offset_);

                typename position_type::vector_type origin_diff;
                for (unsigned i = 0; i < origin_.size(); ++i)
                    origin_diff[i] = int(offset_[i] + 
                                         (offset_[i] > 0.0f ? +0.5f : -0.5f)
                                        );

                // Should work for now. But still, investigate!
                radius2_ = difference2;
                radius_ = new_radius;

                if (radius_ >= data_.get_radius())
                    // TODO: to the nearest power of 2 greater than
                    //       radius_
                    data_.grow_around(origin_diff);

                p.position -= origin_diff;
                origin_ += origin_diff;
            }

            data_[p.position] = p;
            particles_.push_back(p);
        }

        void merge (static_cluster const& other)
        {
            BOOST_FOREACH(particle_type const& p, other.particles_)
            {
                add_particle(p);
            }
            // TODO: Remove particle from other
        }

        bool has_particle_at(position_type const& p) const
        {
            const auto diff = p - origin_;
            if (abs_inf(diff) <= data_.get_radius())
                if (abs2(diff) < radius2_ + 1)
                    return data_[diff].is_initialized();
            return false;
        }

        std::vector<particle_type> const& get_particles () const
        { return particles_; }

        std::size_t get_size () const
        { return get_particles().size(); }

        position_type const& get_center() const
        { return origin_; }

        float get_radius() const
        { return radius_; }

        void move (typename position_type::vector_type const& vec)
        {}

        inline friend
        interaction::result_type interact (static_cluster const& lhs,
                                           static_cluster const& rhs)
        {
            typedef static_cluster::position_type position_type;
            if (
                abs(lhs.get_center() - rhs.get_center()) >
                    lhs.get_radius() + rhs.get_radius()
               )
                return interaction::NONE;

            static_cluster const* smaller_one;
            static_cluster const* bigger_one;

            if (lhs.get_size() > rhs.get_size())
            {
                smaller_one = &rhs;
                bigger_one = &lhs;
            }
            else
            {
                smaller_one = &lhs;
                bigger_one = &rhs;
            }

            for (unsigned m = 0; m < smaller_one->get_size(); ++m)
            {
                for (int k = 0; k < position_type::dimension * 2; ++k)
                {
                    position_type p = smaller_one->get_particles()[m].position
                                    + (2 * (k % 2) - 1)
                                    * get_unit_vector<position_type>(k / 2);

                    if(bigger_one->has_particle_at(p))
                    {
                        return interaction::MERGE;
                    }
                }
            }

            return interaction::NONE;
        }

    protected:
        typename position_type::float_vector_type offset_;
        position_type origin_;
        
        hyper_cube<boost::optional<Particle>, position_type::dimension>
            data_;

        std::vector<particle_type> particles_;

        float radius_, radius2_;
    };

    template <typename Particle>
    class moving_cluster : public static_cluster<Particle>
    {
    public:
        void move (typename Particle::position_type::vector_type const& v)
        {
            static_cluster<Particle>::origin_ += v;
        }
    };

    template <typename Particle>
    class massive_moving_cluster : public moving_cluster<Particle>
    {
    public:
        void move (typename Particle::position_type::vector_type const& v)
        {
            if (++movement_count_ >= moving_cluster<Particle>::get_size())
            {
                moving_cluster<Particle>::move(v);
                movement_count_ = 0;
            }
        }
    private:
        unsigned movement_count_;
    };

}
}

#endif
