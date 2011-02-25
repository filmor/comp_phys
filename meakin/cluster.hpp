#ifndef TRIVIAL_CLUSTER_HPP
#define TRIVIAL_CLUSTER_HPP

#include <vector>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "../print.hpp"
#include "../interaction.hpp"
#include "../hyper_cube.hpp"

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

        static_cluster () : data_(0)
        {}

        void add_particle (Particle p)
        {
            if (particles_.empty())
            {
                ball_center_ = cube_center_ = p.position;
                radius_ = 1;
                radius2_ = 1;
                // ball_carry_ is 0
            }
            else
            {
                auto diff = p.position - ball_center_;

                if (abs2(diff) > radius2_)
                {
                    typedef typename position_type::float_vector_type flt_vec_t;

                    float new_radius = abs(diff);
                    flt_vec_t flt_diff = flt_vec_t(diff) - ball_carry_;

                    // Construct new offset_:
                    // offset_ has to move along relative_position
                    // by 1/2 * (1 - radius / new_radius)
                    ball_carry_ += 0.5f * (1.0f - radius_ / new_radius)
                                   * flt_diff;

                    decltype(ball_center_) ball_center_diff;

                    for (unsigned i = 0; i < ball_center_diff.size(); ++i)
                        ball_center_diff[i] 
                            = int(ball_carry_[i] + 
                                    (ball_carry_[i] > 0.0f ? +0.5f : -0.5f)
                                    );

                    ball_center_ += ball_center_diff;
                    ball_carry_  -= flt_vec_t(ball_center_diff);
                    diff += ball_center_diff;

                    radius2_ = abs2(diff);
                    radius_ = std::sqrt(radius2_);
                }


                if (abs_inf(p.position - cube_center_)
                        > data_.get_radius())
                {
                    //print("growing", cube_center_ - ball_center_);

                    auto diff = ball_center_ - cube_center_;

                    // W00t! This works!!!
                    // print(data_);
                    data_.grow_around(diff, abs_inf(p.position - cube_center_));
                    // print(data_);

                    BOOST_FOREACH( Particle& p2, particles_ )
                    {
                        p2.position -= diff;
                        data_[p2.position]->position -= diff;
                    }

                    cube_center_ = ball_center_;
                }
            }

            // p.position is now relative to cube_center_
            p.position -= cube_center_;
            assert(abs_inf(p.position) <= data_.get_radius());
            // print(p.position, data_.get_radius(), abs_inf(p.position));
            data_[p.position] = p;
            particles_.push_back(p);
            //print(particles_.back().position);
        }

        void merge (static_cluster & other)
        {
            BOOST_FOREACH(particle_type & p, other.particles_)
            {
                p.position += other.cube_center_;
                add_particle(p);
            }
            // TODO: Remove particle from other
        }

        bool has_particle_at(position_type const& p) const
        {
            if (abs2(p - ball_center_) <= radius2_ + 0.1f)
            {
                const auto diff = p - cube_center_;

                if (abs_inf(diff) <= data_.get_radius())
                    return data_[diff].is_initialized();
            }
            return false;
        }

        position_type abs_position(const Particle & p) const
        {
            return p.position + cube_center_;
        }

        std::vector<particle_type> const& get_particles () const
        { return particles_; }

        std::size_t get_size () const
        { return get_particles().size(); }

        position_type const& get_center() const
        { return ball_center_; }

        position_type const& get_cube_center() const
        { return cube_center_; }

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
        typename position_type::float_vector_type ball_carry_;
        position_type ball_center_, cube_center_;
        
        hyper_cube<boost::optional<Particle>, position_type::dimension>
            data_;

        std::vector<particle_type> particles_;

        float radius_, radius2_;
    };

    template <typename Particle>
    class moving_cluster : public static_cluster<Particle>
    {
    public:
        moving_cluster() {}

        explicit moving_cluster(const static_cluster<Particle> & c) : static_cluster<Particle>(c) {}

        void move (typename Particle::position_type::vector_type const& v)
        {
            static_cluster<Particle>::cube_center_ += v;
            static_cluster<Particle>::ball_center_ += v;
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
