#ifndef TRIVIAL_COULOMB_HPP
#define TRIVIAL_COULOMB_HPP

#include "interaction.hpp"

#include <boost/foreach.hpp>
#include <type_traits>

namespace trivial
{

    template <class Particle>
    class charged_particle : public Particle
    {
    public:
        charged_particle() : charge_(0)
        {}

        charged_particle(Particle const& other) : Particle(other), charge_(0)
        {}

        charged_particle(charged_particle const& other) : Particle(other),
                charge_(other.charge_)
        {}
        
        int get_charge() const { return charge_; }
        void set_charge(int f) { charge_ = f; }

    private:
        int charge_;
    };

    template <class Cluster>
    class charged_cluster : public Cluster
    {
    public:
        typedef charged_particle<typename Cluster::particle_type> particle_type;

        charged_cluster() : charge_(0)
        {}

        void add_particle(particle_type const& p)
        {
            charge_ += p.get_charge();
            Cluster::add_particle(p);
        }

        int get_charge() const { return charge_; }
    private:
        int charge_;
    };

    template <typename Base>
    void interact (charged_cluster<Base> const& part1,
                   charged_cluster<Base> const& part2,
                   typename charged_cluster<Base>::interaction_type& state)
    {
        auto force = (part2.get_center() - part1.get_center())
                     * part1.get_charge() * part2.get_charge();

        force /= abs2(force);

        for (unsigned i = 0; i < 2 * part1.get_center().size(); ++i)
            state.mult_movement(i, 1.0f + ((i % 2) ? -1 : +1) * force[i/2]);

        interact((Base const&)(part1), (Base const&)(part2), state);
    }

    template <typename Base, typename BaseC>
    void interact (charged_particle<Base> const& part1,
                   charged_cluster<BaseC> const& part2,
                   typename charged_particle<Base>::interaction_type& state)
    {
        auto force = (part2.get_center() - part1.position)
                     * part1.get_charge() * part2.get_charge();

        if (abs2(force) != 0)
        {
            force /= abs2(force);

            for (unsigned i = 0; i < 2 * part1.position.size(); ++i)
                state.mult_movement(i, 1.0f + ((i % 2) ? -1 : +1) * force[i/2]);
        }

        interact((Base const&)(part1), (BaseC const&)(part2), state);
    }

    template <typename Base>
    void interact (charged_particle<Base> const& part1,
                   charged_particle<Base> const& part2,
                   typename charged_particle<Base>::interaction_type& state)
    {
        auto force = (part2.position - part1.position)
                     * part1.get_charge() * part2.get_charge();

        force /= abs2(force);

        for (unsigned i = 0; i < 2 * part1.position.size(); ++i)
            state.mult_movement(i, 1.0f + ((i % 2) ? -1 : +1) * force[i/2]);

        interact((Base const&)(part1), (Base const&)(part2), state);
    }

    template <typename Base>
    class charging_updater : public Base
    {
    public:
        charging_updater () : dist_(0, 1) {}

        template <class RandomNumberGenerator>
        void operator()(std::vector<typename Base::particle_type>& p,
                        std::vector<typename Base::cluster_type>& c,
                        RandomNumberGenerator& rng)
        {
            Base::operator()(p, c, rng);

            if (!p.empty())
            {
                BOOST_FOREACH(auto& part, p)
                {
                    if (part.get_charge() == 0)
                    {
                        part.set_charge(dist_(rng) == 1 ? 1 : -1);
                    }
                }
            }
        }

    private:
        std::uniform_int_distribution<int> dist_;
    };

}

#endif
