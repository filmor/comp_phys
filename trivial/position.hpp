#ifndef TRIVIAL_POSITION_HPP
#define TRIVIAL_POSITION_HPP

#include <array>
#include <cmath>
#include <ostream>

namespace trivial
{
	//abc
	template<unsigned N>
	class position
	{
		public:
			static const unsigned dimension = N;

			//sub classes shall define static const std::vector<*> unit_vectors
			//sub classes shall define vector space operations
		
			position(const std::array<float, N> & coord = std::array<float, N>({})) : coord_(coord) {};

			bool operator==(const position & p) const
			{
				for(unsigned n = 0; n < N; ++n)
					if(std::abs(this->coord_[n] - p.coord_[n]) > 0.5f)  // OK
					//if(std::floor(this->coord_[n] + 0.5f) != std::floor(p.coord_[n] + 0.5f))  // exact
					//if(this->coord_[n] != p.coord_[n])	// fast
						return false;
				return true;
			}

			const std::array<float, N> & to_array() const
			{
				return coord_;
			}

			const std::array<int, N> to_int_array() const
			{
				std::array<int, N> c;
				for(unsigned n = 0; n < N; ++n)
					c[n] = std::floor(coord_[n] + 0.5f);
				return c;
			}

			void round()
			{
				for(unsigned n = 0; n < N; ++n)
					coord_[n] = std::floor(coord_[n] + 0.5f);
			}

		protected:
			std::array<float, N> coord_;

			template<unsigned M>
			friend std::ostream & operator<<(std::ostream & out, const position<M> & p);

			template<unsigned M>
			friend float abs(const position<M> & p);

			template<unsigned M>
			friend float abs2(const position<M> & p);
	};

	template<unsigned N>
	float abs(const position<N> & p)
	{
		return std::sqrt(abs2(p));
	}

	template<unsigned N>
	float abs2(const position<N> & p)
	{
		float d = 0;
		for(unsigned n = 0; n < N; ++n)
			d += p.coord_[n] * p.coord_[n];
		return d;
	}

	template<unsigned N>
	std::ostream & operator<<(std::ostream & out, const position<N> & p)
	{
		out << "(";
		for(unsigned n = 0; n < N - 1; ++n)
        		out << p.coord_[n] << ",";
		out << p.coord_[N - 1] << ")";
        	return out;
	}
}

#endif

