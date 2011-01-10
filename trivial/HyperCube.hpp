#ifndef TRIVIAL_HYPERCUBE_HPP
#define TRIVIAL_HYPERCUBE_HPP

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

namespace trivial
{
	template<class T, unsigned N>
	class HyperCube
	{
		public:
			HyperCube(unsigned size, const std::array<int, N> & offset) : size_(size), offset_(offset), map_(pow(size, N)) {}

			void shift_offset(const std::array<int, N> & shift)
			{
				for(unsigned n = 0; n < N; n++)
					offset_[n] += shift[n];
			}

			typename std::vector<T>::reference operator()(const std::array<int, N> & key)
			{
				for(;;)
				{
					bool f = false;
					for(unsigned n = 0; n < N && !f; n++)
						f = (key[n] >= offset_[n] + size_ || key[n] < offset_[n]);
					if(f)
						resize(1.5);
					else break;
				}

				return map_[index(key)];
			}

			void clear()
			{
				std::fill(map_.begin(), map_.end(), T());
			}
			
			void print()
			{
				if(N != 2)
					throw "not supported";
	
				std::cout<<"("<<offset_[0]<<","<<offset_[1]<<") "<<size_<<std::endl;
				std::array<int, 2> p;
				for(p[1] = offset_[1] + size_ - 1; p[1] >= offset_[1]; --p[1])
				{
					for(p[0] = offset_[0]; p[0] < offset_[0] + size_; ++p[0])
						std::cout << (map_[index(p)] ? "x" : "_");
					std::cout << std::endl;
				}
			}

		private:
			std::vector<T> map_;
			std::array<int, N> offset_;
			int size_;

			inline const int index(const std::array<int, N> & key, const unsigned size, const std::array<int, N> & offset) const
			{
				unsigned index = 0;
				for(unsigned n = 0; n < N; ++n)
					index = size * index + (key[n] - offset[n]);
				return index;
			}

			inline const int index(const std::array<int, N> & key) const
			{
				return index(key, size_, offset_);
			}

			void resize(const float factor)
			{
				if(factor < 1)
					throw "not supported";

				int new_size = size_ * factor;
				int shift = (new_size - size_) / 2;
				std::array<int, N> new_offset = offset_;
				for(unsigned n = 0; n < N; ++n)
					new_offset[n] -= shift;

				map_.resize(pow(new_size, N));

				std::array<int, N> key;
				move_content(key, new_size, new_offset, 0);

				size_ = new_size;
				offset_ = new_offset;
			}

			void move_content(std::array<int, N> & key, const unsigned new_size, const std::array<int, N> & new_offset, const unsigned n)
			{
				key[n] = size_ + offset_[n] - 1;
				while(key[n] >= offset_[n])
				{
					if(n < N - 1)
						move_content(key, new_size, new_offset, n + 1);
					else
					{
						unsigned old_index = index(key);
						unsigned new_index = index(key, new_size, new_offset);
						map_[new_index] = map_[old_index];
						if(new_index != old_index)
							map_[old_index] = T();
					}

					--key[n];
				}			
			}
	};
}

#endif
