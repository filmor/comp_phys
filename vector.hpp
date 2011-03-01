#ifndef TRIVIAL_VECTOR_HPP
#define TRIVIAL_VECTOR_HPP

#include <cmath>
#include <array>
#include <ostream>
#include <boost/operators.hpp>

namespace trivial 
{
    template <unsigned N, typename T = int>
    struct vector : std::array<T, N>
                  , boost::additive1<vector<N, T>>
                  , boost::multiplicative2<vector<N, T>, T>
                  , boost::equality_comparable<vector<N, T>>
    {
        typedef std::array<T, N> base_type;
        static const unsigned dimension = N;

        vector ()
        {
            for (auto i = this->begin(); i != this->end(); ++i)
                *i = 0;
        }

        template <typename T2>
        explicit vector (vector<N, T2> const& other)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] = other[i];
        }

        vector& operator*= (T val)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] *= val;
            return *this;
        }

        bool operator== (vector const& other) const
        {
            for (unsigned i = 0; i < N; ++i)
                if ((*this)[i] != other[i])
                    return false;
            return true;
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

        vector& operator/= (T val)
        {
            return *this *= 1/val;
        }
    };

    template <unsigned N, typename T>
    T abs2 (vector<N, T> const& v)
    {
        T res = 0;
        for (unsigned i = 0; i < v.size(); ++i)
            res += v[i] * v[i];
        return res;
    }

    template <unsigned N, typename T>
    T abs_1 (vector<N, T> const& v)
    {
        T res = 0;
        for (unsigned i = 0; i < v.size(); ++i)
            res += std::abs(v[i]);
        return res;
    }

    template <unsigned N, typename T>
    T abs_inf (vector<N, T> const& v)
    {
        T res = 0;
        for (unsigned i = 0; i < v.size(); ++i)
            res = std::max(res, abs(v[i]));
        return res;
    }

    template <unsigned N, typename T>
    float abs (vector<N, T> const& v)
    {
        return std::sqrt(float(abs2(v)));
    }
    
    template <typename T>
    T const& get_unit_vector (unsigned n)
    {
        static T unit_vectors[T::dimension + 1];
        static bool initialized = false;

        if (!initialized)
        {
            for (unsigned i = 0; i < T::dimension; ++i)
                for (unsigned j = 0; j < T::dimension; ++j)
                    unit_vectors[i][j] = (i == j) ? 1 : 0;
            initialized = true;
        }

        return unit_vectors[std::min(n, unsigned(T::dimension))];
    }

    template<unsigned N, typename T>
    std::ostream & operator<<(std::ostream & out, const vector<N, T> & p)
    {
        out << "(";
        for(unsigned n = 0; n < N - 1; ++n)
                out << p[n] << ",";
        out << p[N - 1] << ")";
            return out;
    }

}

#endif
