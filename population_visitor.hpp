#ifndef TRIVIAL_POPULATION_VISITOR_HPP
#define TRIVIAL_POPULATION_VISITOR_HPP

#include "visitor.hpp"
#include "print.hpp"

namespace trivial
{
    template<class World>
    class population_visitor : public const_visitor<World>
    {
        void visit(World const& world)
        {
            unsigned sum = 0;

            for (unsigned n = 0; n < world.get_clusters().size(); ++n)
                sum += world.get_clusters()[n].get_particles().size();

            print("Currently ", world.get_particles().size(),
                  "free particle(s) and", sum, "in",
                  world.get_clusters().size(), "cluster(s)."
                 );
        }
    };
}

#endif
