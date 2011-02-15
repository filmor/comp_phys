#ifndef TRIVIAL_EDEN_PARTICLE_HPP
#define TRIVIAL_EDEN_PARTICLE_HPP

#include "../meakin/particle.hpp"

namespace trivial
{
namespace eden
{

    template <typename Position>
    class sticky_particle : public meakin::sticky_particle<Position>
    {
    public:
        typedef Position position_type;

        position_type parent;
        float score;

        sticky_particle(const Position& pos) : meakin::sticky_particle<Position>(pos), score(0) {}

        friend interaction::result_type     // shouldn't be neccessary to rewrite this...
            interact(const sticky_particle& particle,
                     const static_cluster<sticky_particle>& cluster)
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
    };
}
}

#endif
