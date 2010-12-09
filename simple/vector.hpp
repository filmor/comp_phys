#ifndef SIMPLE_VECTOR_HPP
#define SIMPLE_VECTOR_HPP

#include <array>

namespace simple
{
    
    template <unsigned N>
    struct vector : public std::array<float, N>
    {
        typedef std::array<float, N> base_type;
        vector (std::initializer_list<float> args)
        {
            unsigned k = 0;
            for (auto i = args.begin(); i != args.end(); ++i)
            {
                if (k == N)
                    break;
                base_type::operator[](k) = *i;
                ++k;
            }
        }
    };

}

#endif
