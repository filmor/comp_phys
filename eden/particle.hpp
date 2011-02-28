#ifndef TRIVIAL_EDEN_PARTICLE_HPP
#define TRIVIAL_EDEN_PARTICLE_HPP

#include "cluster.hpp"
#include "../interaction.hpp"

namespace trivial
{
namespace eden
{
    template <typename Position>
    class static_particle
    {
    public:
        typedef Position position_type;
        typedef standard_interaction<Position> interaction_type;

        //TODO encapsulate
        position_type position; 
        position_type parent;
        float score;

        static_particle(const Position& pos) : position(pos), score(0) {}

        friend
        void interact (const static_particle& particle,
                       const static_cluster<static_particle>& cluster,
                       interaction_type& state)
        {
            for (unsigned n = 0; n < Position::dimension * 2; ++n)
                if(cluster.has_particle_at(particle.position
                                            + (2 * (n % 2) - 1)
                                            * get_unit_vector<Position>(n / 2))
                  )
                    state.set_merge();
        }

        friend
        void interact (const static_particle& particle1,
                       const static_particle& particle2,
                       interaction_type& state)
        {}

        void move(typename Position::vector_type const& vec) {}
    };
}
}

#endif
