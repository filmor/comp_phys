#ifndef TRIVIAL_TUBE_PARTICLE_HPP
#define TRIVIAL_TUBE_PARTICLE_HPP

#include <vector>

#include "../interaction.hpp"
#include "cluster.hpp"

namespace trivial
{
namespace tube
{

    template <typename Position, unsigned Slowness>
    class flowing_particle
    {
    public:
        typedef Position position_type;
        typedef standard_interaction<Position> interaction_type;

        static const unsigned slowness = Slowness;

        // TODO: Encapsulate or remove
        position_type position; 

        flowing_particle(const Position& pos) : position(pos), wait_(1) {}

        friend
        void interact (const flowing_particle& particle,
                       const flowing_cluster<flowing_particle>& cluster,
                       interaction_type& state)
        {
            // TODO: Stickyness
            for (unsigned n = 0; n < Position::dimension * 2; ++n)
            {
                const Position p = particle.position + (2 * (n % 2) - 1) 
                                 * get_unit_vector<Position>(n / 2);
                if (cluster.has_particle_at(p))
                    state.set_merge();
            }
        }

        friend
        void interact (const flowing_particle& particle1,
                       const flowing_particle& particle2,
                       interaction_type& state)
        {
            if (abs_1(particle1.position - particle2.position) <= 1)
                state.set_merge();
        }

        void move (typename Position::vector_type const& vec)
        {
            if(--wait_ == 0)
            // TODO should be done with probabilities once interactions are
            //      implemented
            {
                ++position[0];
                wait_ = Slowness;
            }
            else position += vec;
        }

    private:
        unsigned wait_;
    };
}
}

#endif
