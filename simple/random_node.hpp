#ifndef SIMPLE_RANDOM_NODE_HPP
#define SIMPLE_RANDOM_NODE_HPP

#include <boost/random/normal_distribution.hpp>
#include "vector.hpp"
#include <iostream>

namespace simple
{

    template <typename Lattice, typename Rng>
    typename Lattice::node_type random_node (typename Lattice::node_type center,
                                             float radius, Rng& gen)
    {
        using namespace boost;

         // Winkel gleichverteilt (auf der n-Sphäre):
         //  x, y, z etc. normalverteilt, dann normalisieren

        normal_distribution<float> dist (radius, radius / 2);

        variate_generator<Rng&, normal_distribution<float>> normal(gen, dist);

        vector<Lattice::dimensions> vec = center.get_vector();

        for (unsigned i = 0; i < Lattice::dimensions; ++i)
        {
            if (normal() > radius)
                vec[i] = normal();
            else
                vec[i] = -normal();
        }

        return typename Lattice::node_type(vec);
    }

}

#endif
