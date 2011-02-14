#ifndef TRIVIAL_STATISTICS_VISITOR_HPP
#define TRIVIAL_STATISTICS_VISITOR_HPP

#include "visitor.hpp"
#include "vector.hpp"
#include "print.hpp"

namespace trivial
{
    template<class World>
    class rog_visitor : public const_visitor<World>
    {
        void visit(World const& world)
        {
            for(unsigned n = 0; n < world.get_clusters().size(); ++n)
            {
                const typename World::cluster_type & c = world.get_clusters()[n];

                typedef typename World::position_type::float_vector_type flt_vec_t;
                flt_vec_t center;

                for(unsigned m = 0; m < c.get_size(); ++m)
                    center += flt_vec_t(c.get_particles()[m].position);

                center /= c.get_size();

                float rog = 0.f;
                for(unsigned m = 0; m < c.get_size(); ++m)
                    rog += abs2(flt_vec_t(c.get_particles()[m].position) - center);
                rog /= c.get_size();
                rog = std::sqrt(rog);

                print(c.get_size(), rog);
            }
        }
    };
}

#endif
