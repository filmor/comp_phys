#ifndef TRIVIAL_PARTICLE_HPP
#define TRIVIAL_PARTICLE_HPP

#include <vector>

#include "../interaction.hpp"
#include "cluster.hpp"

namespace trivial
{
namespace meakin
{

    template <typename Position, typename Interaction = standard_interaction<Position>>
    class sticky_particle
    {
    public:
        typedef Position position_type;
        typedef Interaction interaction_type;

        // TODO: Encapsulate or remove
        position_type position; 

        sticky_particle(const Position& pos) : position(pos) {}

        friend 
        void interact(const sticky_particle& particle,
                      const static_cluster<sticky_particle>& cluster,
                      interaction_type& state)
        {
            for (unsigned n = 0; n < Position::dimension * 2; ++n)
            {
                const Position p = particle.position + (2 * (n % 2) - 1) 
                                 * get_unit_vector<Position>(n / 2);
                
                if (cluster.has_particle_at(p))
                {
                    state.set_merge();
                    return;
                }
            }
        }

        friend
        void interact(const sticky_particle& particle1,
                      const sticky_particle& particle2,
                      interaction_type& state)
        {
            if (abs_1(particle1.position - particle2.position) <= 1)
                state.set_merge();
        }

        void move(typename Position::vector_type const& vec)
        {
            position += vec;
        }
    };
}
}

#endif
