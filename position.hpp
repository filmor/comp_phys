#ifndef TRIVIAL_POSITION_HPP
#define TRIVIAL_POSITION_HPP

#include "vector.hpp"

namespace trivial
{

    template <unsigned N>
    class position : public vector<N>
    {
    public:
        typedef vector<N, int> vector_type;
        typedef vector<N, float> float_vector_type;

        position () {}

        position (const vector<N>& other)
            : vector<N> (other)
        {}
    };

}

#endif

