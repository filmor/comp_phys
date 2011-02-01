#ifndef TRIVIAL_POSITION_HPP
#define TRIVIAL_POSITION_HPP

#include "vector.hpp"
#include <ostream>

namespace trivial
{

	template <unsigned N>
	class position : public vector<N>
	{
    public:
        position () {}

        position (const vector<N>& other)
            : vector<N> (other)
        {}

        static const unsigned dimension = N;

        template<unsigned M>
        friend std::ostream & operator<<(std::ostream & out, const position<M> & p);
	};

	template<unsigned N>
	std::ostream & operator<<(std::ostream & out, const position<N> & p)
	{
		out << "(";
		for(unsigned n = 0; n < N - 1; ++n)
        		out << p[n] << ",";
		out << p[N - 1] << ")";
        	return out;
	}
}

#endif

