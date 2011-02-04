#ifndef TRIVIAL_PARTICLE_HPP
#define TRIVIAL_PARTICLE_HPP

#include <vector>

#include "interaction.hpp"
#include "position.hpp"
#include "cluster.hpp"
#include "visitor.hpp"

namespace trivial
{

    template <typename Position>
    class sticky_particle
    {
    public:
        typedef Position position_type;

        // TODO: Encapsulate or remove
        position_type position; 

        sticky_particle(const Position& pos) : position(pos) {}

        friend interaction::result_type
            interact(const sticky_particle& particle,
                     const cluster<sticky_particle>& cluster)
        {
            // TODO: Stickyness
            for (unsigned n = 0; n < Position::dimension * 2; ++n)
            {
                const Position p = particle.position + (2 * (n % 2) - 1) 
                                 * get_unit_vector<Position>(n / 2);
                if (cluster.has_particle_at(p))
                    return interaction::MERGE;
            }
            return interaction::NONE;
        }

        friend interaction::result_type
            interact(const sticky_particle& particle1,
                     const sticky_particle& particle2)
        {
            if (abs_1(particle1.position - particle2.position) <= 1)
                return interaction::MERGE;
            else
                return interaction::NONE;
        }

        void move(typename Position::vector_type const& vec)
        {
            position += vec;
        }

        TRIVIAL_DEFINE_VISITABLE(sticky_particle);
    };
}

#endif
