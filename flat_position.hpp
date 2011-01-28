#ifndef TRIVIAL_FLAT_POSITION_HPP
#define TRIVIAL_FLAT_POSITION_HPP

#include <vector>
#include <boost/operators.hpp>

#include "position.hpp"

namespace trivial
{
	template<unsigned N>
	class flat_position : public position<N>, boost::addable1<flat_position<N>, boost::subtractable<flat_position<N>, boost::multipliable2<flat_position<N>, float, boost::dividable2<flat_position<N>, float>>>>
	{
		public:
			static const std::vector<flat_position> unit_vectors;

			flat_position(const std::array<float, N> & coord = std::array<float, N>({})) : position<N>(coord) {};

			flat_position & operator+=(const flat_position & p)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] += p.coord_[n];
				return *this;
			}

			flat_position & operator-=(const flat_position & p)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] -= p.coord_[n];
				return *this;
			}

			flat_position & operator*=(const float f)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] *= f;
				return *this;
			}

			flat_position & operator/=(const float f)
			{
				return *this *= 1/f;
			}
			
		private:
			static std::vector<flat_position> create_unit_vectors();
	};

	template<unsigned N>
	const std::vector<flat_position<N>> flat_position<N>::unit_vectors = flat_position::create_unit_vectors();

	template<unsigned N>
	std::vector<flat_position<N>> flat_position<N>::create_unit_vectors()
	{
		std::vector<flat_position> unit_vectors(N);
		for(unsigned n = 0; n < N; ++n)
			unit_vectors[n].coord_[n] = 1;
		return unit_vectors;
	}
}

#endif
