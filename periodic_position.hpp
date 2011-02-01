#ifndef TRIVIAL_PERIODIC_POSITION_HPP
#define TRIVIAL_PERIODIC_POSITION_HPP

#include <vector>
#include <boost/operators.hpp>

#include "position.hpp"

namespace trivial
{
	template<unsigned N, std::size_t Size>
	class periodic_position : public position<N>
	{
    public:
        periodic_position () {}

        periodic_position (vector<N> const& other)
        {
            for (unsigned i = 0; i < N; ++i)
                (*this)[i] = other[i];
            clean_coord();
        }

        periodic_position& operator+=(const periodic_position& p)
        {
            for(unsigned n = 0; n < N; ++n)
                (*this)[n] += p[n];
            clean_coord();
            return *this;
        }

        periodic_position& operator-=(const periodic_position& p)
        {
            for(unsigned n = 0; n < N; ++n)
                (*this)[n] -= p.coord_[n];
            clean_coord();
            return *this;
        }

        periodic_position& operator*=(const float f)
        {
            for(unsigned n = 0; n < N; ++n)
                this->coord_[n] *= f;
            clean_coord();
            return *this;
        }

    private:
        void clean_coord()
        {
            for(unsigned n = 0; n < N; ++n)
            {
                // should be faster than calculation because always
                // close to valid position
                while ((*this)[n] >= int(Size / 2))
                    (*this)[n] -= Size;
                while ((*this)[n] < -(int(Size) + 1) / 2)
                    (*this)[n] += Size;
            }
        }
	};

}

#endif
