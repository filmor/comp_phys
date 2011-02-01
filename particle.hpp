#ifndef TRIVIAL_PARTICLE_HPP
#define TRIVIAL_PARTICLE_HPP

#include <vector>

#include "position.hpp"
#include "cluster.hpp"
#include "visitor.hpp"

namespace trivial
{
	template<typename Position>
	class particle
	{
    public:
        enum result_type { RESULT_NONE, RESULT_JOIN };

        typedef Position position_type;

        Position position;	

        particle(const Position & pos) : position(pos) {};

        inline friend
        result_type interact(const particle&, const particle&)
        {
            return RESULT_NONE;
        }

        inline friend
        result_type interact(const particle&, const cluster<particle>&)
        {
            return RESULT_NONE;
        }

        inline friend
        std::ostream& operator<< (std::ostream& os, particle const& part)
        {
            return os << ".";
        }

        TRIVIAL_DEFINE_VISITABLE(particle);

        void move() {}
	};

	template <typename Position>
	class sticky_particle : public particle<Position> // TODO sticky factor
	{
    public:
        sticky_particle(const Position& pos) : particle<Position>(pos) {};

        inline friend
        char interact(const sticky_particle& particle,
                      const cluster<sticky_particle>& cluster)
        {
            for (unsigned n = 0; n < Position::dimension * 2; ++n)
            {
                const Position p = particle.position + (2 * (n % 2) - 1) 
                                 * get_unit_vector<Position>(n / 2);
                if (cluster.has_particle_at(p))
                    return sticky_particle::RESULT_JOIN;
            }
            return sticky_particle::RESULT_NONE;
        }

        inline friend
        char interact(const sticky_particle& particle1,
                      const sticky_particle& particle2)
        {
            if (abs2(particle1.position - particle2.position) <= 1.1)
                return sticky_particle::RESULT_JOIN;
            else
                return sticky_particle::RESULT_NONE;
        }

        TRIVIAL_DEFINE_VISITABLE(sticky_particle);
	};
}

#endif
