#ifndef TRIVIAL_PERIODIC_POSITION_HPP
#define TRIVIAL_PERIODIC_POSITION_HPP

#include <vector>
#include <boost/operators.hpp>

#include "position.hpp"

namespace trivial
{
	template<unsigned N, int Size>
	class periodic_position : public position<N>, boost::addable1<periodic_position<N, Size>, boost::subtractable<periodic_position<N, Size>, boost::multipliable2<periodic_position<N, Size>, float, boost::dividable2<periodic_position<N, Size>, float>>>>
	{
		public:
			static const std::vector<periodic_position> unit_vectors;

			periodic_position(const std::array<float, N> & coord = std::array<float, N>({})) : position<N>(coord)
			{
				clean_coord();
			};

			periodic_position & operator+=(const periodic_position & p)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] += p.coord_[n];
				clean_coord();
				return *this;
			}

			periodic_position & operator-=(const periodic_position & p)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] -= p.coord_[n];
				clean_coord();
				return *this;
			}

			periodic_position & operator*=(const float f)
			{
				for(unsigned n = 0; n < N; ++n)
					this->coord_[n] *= f;
				clean_coord();
				return *this;
			}

			periodic_position & operator/=(const float f)
			{
				return *this *= 1/f;
			}

		private:
			void clean_coord()
			{
				for(unsigned n = 0; n < N; ++n)
				{
					while(this->coord_[n] >= Size / 2)	//should be faster than calculation because always close to valid position
						this->coord_[n] -= Size;
					while(this->coord_[n] < -(Size + 1) / 2)
						this->coord_[n] += Size;
				}
			}

			static std::vector<periodic_position> create_unit_vectors();
	};

	template<unsigned N, int Size>
	const std::vector<periodic_position<N, Size>> periodic_position<N, Size>::unit_vectors = periodic_position::create_unit_vectors();

	template<unsigned N, int Size>
	std::vector<periodic_position<N, Size>> periodic_position<N, Size>::create_unit_vectors()
	{
		std::vector<periodic_position> unit_vectors(N);
		for(unsigned n = 0; n < N; ++n)
			unit_vectors[n].coord_[n] = 1;
		return unit_vectors;
	}
}

#endif
