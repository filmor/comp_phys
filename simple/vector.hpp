#ifndef SIMPLE_VECTOR_HPP
#define SIMPLE_VECTOR_HPP

#include <cmath>
#include <array>
#include <boost/operators.hpp>

namespace simple
{
    template <unsigned N>
    struct vector : std::array<float, N>
                  , boost::addable1<vector<N>
                  , boost::subtractable<vector<N>
                  , boost::multipliable2<vector<N>, float
                  , boost::dividable2<vector<N>, float>
                    >>>
    {
        vector ()
        {
            for (auto i = this->begin(); i != this->end(); ++i)
                *i = 0.f;
        }

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

        vector& operator*= (float val)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] *= val;
            return *this;
        }

        vector& operator+= (vector const& other)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] += other[i];
            return *this;
        }

        vector& operator-= (vector const& other)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] -= other[i];
            return *this;
        }

        vector& operator/= (float val)
        {
            return *this *= 1/val;
        }
    };

    template <unsigned N>
    float abs (vector<N> const& v)
    {
        float res = 0.f;
        for (unsigned i = 0; i < v.size(); ++i)
            res += v[i] * v[i];
        return std::sqrt(res);
    }

    template <unsigned N>
    float dist (const vector<N>& vec1, const vector<N>& vec2)
    {
        return abs(vec1 - vec2);
    }
    
}

#endif
