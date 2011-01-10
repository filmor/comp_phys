#ifndef TRIVIAL_TOPOLOGY_HPP
#define TRIVIAL_TOPOLOGY_HPP

#include <array>
#include <cmath>

namespace trivial
{
	//prototype. not implemented
	template<unsigned N>
	class Topology
	{
		public:
			static const unsigned dimensions = N;
			static const unsigned num_neighbours = 2 * N;
			typedef std::array<int, N> position_type;
			typedef std::array<position_type, 2 * N> neighbours_type;
			typedef float distance_type;
			struct bounds_type
			{
				position_type min, max, center;
				distance_type radius;
			};

			static distance_type dist(const position_type & p0, const position_type & p1);
			static bool intersect(const position_type & p0, const position_type & p1);
			static bool are_neighbours(const position_type & p0, const position_type & p1);
			static bool intersect(const bounds_type & b, const position_type & p);
			static void add_to_bounds(bounds_type & b, const position_type & p);
			static void init_bounds(bounds_type & b, const position_type & p);
			static const neighbours_type & neighbours(const position_type & p0);
			static const neighbours_type & neighbours(const bounds_type & b);

		private:
			static neighbours_type buf_neighbours_;
	};

	template<unsigned N>
	class FlatTopology
	{
		public:
			static const unsigned dimensions = Topology<N>::dimensions;
			static const unsigned num_neighbours = Topology<N>::num_neighbours;
			typedef typename Topology<N>::position_type position_type;
			typedef typename Topology<N>::bounds_type bounds_type;
			typedef typename Topology<N>::neighbours_type neighbours_type;
			typedef typename Topology<N>::distance_type distance_type;

			static distance_type dist(const position_type & p0, const position_type & p1)
			{
				distance_type d2 = 0;
				int d;
			
				for(unsigned n = 0; n < N; ++n)
				{
					d = abs(p0[n] - p1[n]);
					d2 += d * d;
				}
				return sqrt(d2);
			}

			static bool intersect(const position_type & p0, const position_type & p1)
			{
				for(unsigned n = 0; n < N; ++n)
					if(p0[n] != p1[n])
						return false;
				return true;
			}

			static bool intersect(const bounds_type & b, const position_type & p)
			{
				for(unsigned n = 0; n < N; ++n)
					if(b.min[n] > p[n] || b.max[n] < p[n])
						return false;
				return true;
			}

			static bool are_neighbours(const position_type & p0, const position_type & p1)
			{
				int d;
				bool deviation = false;
				for(unsigned n = 0; n < N; ++n)
				{
					d = abs(p0[n] - p1[n]);
					if(d == 1)
						if(!deviation)
							deviation = true;
						else return false;
					else if(d != 0)
						return false;								
				}
				return deviation;
			}

			static void add_to_bounds(bounds_type & b, const position_type & p)
			{
				bool change = false;
				for(unsigned n = 0; n < N; ++n)
				{
					if(b.min[n] > p[n])
					{
						b.min[n] = p[n];
						b.center[n] = (b.max[n] + b.min[n]) / 2;
						change = true;
					}
					else if(b.max[n] < p[n])
					{
						b.max[n] = p[n];
						b.center[n] = (b.max[n] + b.min[n]) / 2;
						change = true;
					}
				}
				if(change)
					b.radius = dist(b.max, b.center);
			}

			static void init_bounds(bounds_type & b, const position_type & p)
			{
				b.min = b.max = b.center = p;
				b.radius = 1;
			}

			static const neighbours_type & neighbours(const position_type & p)
			{
				for(unsigned n = 0; n < N; ++n)
				{
					FlatTopology<N>::buf_neighbours_[2 * n + 0] = p;
					++FlatTopology<N>::buf_neighbours_[2 * n + 0][n];
					FlatTopology<N>::buf_neighbours_[2 * n + 1] = p;
					--FlatTopology<N>::buf_neighbours_[2 * n + 1][n];
				}
				return FlatTopology<N>::buf_neighbours_;
			}

			static const neighbours_type & neighbours(const bounds_type & b)
			{
				for(unsigned n = 0; n < N; ++n)
				{
					FlatTopology<N>::buf_neighbours_[2 * n + 0] = b.center;
					++FlatTopology<N>::buf_neighbours_[2 * n + 0][n];
					FlatTopology<N>::buf_neighbours_[2 * n + 1] = b.center;
					--FlatTopology<N>::buf_neighbours_[2 * n + 1][n];
				}
				return FlatTopology<N>::buf_neighbours_;
			}

		private:
			static neighbours_type buf_neighbours_;
	};

	template<unsigned N, unsigned SIZE>
	class FlatBoundedTopology : public FlatTopology<N>
	{
		public:
			static const unsigned dimensions = FlatTopology<N>::dimensions;
			static unsigned num_neighbours;
			typedef typename FlatTopology<N>::position_type position_type;
			typedef typename FlatTopology<N>::bounds_type bounds_type;
			typedef typename FlatTopology<N>::neighbours_type neighbours_type;
			typedef typename FlatTopology<N>::distance_type distance_type;

			static const neighbours_type & neighbours(const position_type & p)
			{
				unsigned k = 0;
				for(unsigned n = 0; n < N; ++n)
				{
					if(p[n] != SIZE - 1)
					{
						FlatBoundedTopology<N, SIZE>::buf_neighbours_[k] = p;
						++FlatBoundedTopology<N, SIZE>::buf_neighbours_[k][n];
						++k;
					}
					if(p[n] != 0)
					{
						FlatBoundedTopology<N, SIZE>::buf_neighbours_[k] = p;
						--FlatBoundedTopology<N, SIZE>::buf_neighbours_[k][n];
						++k;
					}
				}
				FlatBoundedTopology<N, SIZE>::num_neighbours = k;
				return FlatBoundedTopology<N, SIZE>::buf_neighbours_;
			}

			static const neighbours_type & neighbours(const bounds_type & b)
			{
				unsigned k = 0;
				for(unsigned n = 0; n < N; ++n)
				{
					if(b.max[n] != SIZE - 1)
					{
						FlatBoundedTopology<N, SIZE>::buf_neighbours_[k] = b.center;
						++FlatBoundedTopology<N, SIZE>::buf_neighbours_[k][n];
						++k;
					}
					if(b.min[n] != 0)
					{
						FlatBoundedTopology<N, SIZE>::buf_neighbours_[k] = b.center;
						--FlatBoundedTopology<N, SIZE>::buf_neighbours_[k][n];
						++k;
					}
				}
				FlatBoundedTopology<N, SIZE>::num_neighbours = k;
				return FlatBoundedTopology<N, SIZE>::buf_neighbours_;
			}

		private:
			static neighbours_type buf_neighbours_;
	};

	//bounds not implemented yet. hypercube mapping has to be taken care of!
	template<unsigned N, unsigned SIZE>
	class FlatPeriodicTopology : public FlatTopology<N>
	{
		public:
			static const unsigned dimensions = FlatTopology<N>::dimensions;
			static const unsigned num_neighbours = FlatTopology<N>::num_neighbours;
			typedef typename FlatTopology<N>::position_type position_type;
			typedef typename FlatTopology<N>::bounds_type bounds_type;
			typedef typename FlatTopology<N>::neighbours_type neighbours_type;
			typedef typename FlatTopology<N>::distance_type distance_type;

			static distance_type dist(const position_type & p0, const position_type & p1)
			{
				distance_type d2 = 0;
				int d;
			
				for(unsigned n = 0; n < N; ++n)
				{
					d = abs(p0[n] - p1[n]);
					if(d > SIZE / 2)
						d = SIZE - d;
					d2 += d * d;
				}
				return sqrt(d2);
			}

			static bool are_neighbours(const position_type & p0, const position_type & p1)
			{
				int d;
				bool deviation = false;
				for(unsigned n = 0; n < N; ++n)
				{
					d = abs(p0[n] - p1[n]);
					if(d == 1 || d == SIZE - 1)
						if(!deviation)
							deviation = true;
						else return false;
					else if(d != 0)
						return false;								
				}
				return deviation;
			}

			static const neighbours_type & neighbours(const position_type & p)
			{
				for(unsigned n = 0; n < N; ++n)
				{
					FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 0] = p;
					++FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 0][n];
					if(FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 0][n] == SIZE)
						FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 0][n] = 0;
					FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 1] = p;
					--FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 1][n];
					if(FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 1][n] == -1)
						FlatPeriodicTopology<N, SIZE>::buf_neighbours_[2 * n + 1][n] = SIZE - 1;
				}
				return FlatPeriodicTopology<N, SIZE>::buf_neighbours_;
			}

		private:
			static neighbours_type buf_neighbours_;
	};

	template<unsigned N>
	typename FlatTopology<N>::neighbours_type FlatTopology<N>::buf_neighbours_;
	template<unsigned N, unsigned SIZE>
	typename FlatBoundedTopology<N, SIZE>::neighbours_type FlatBoundedTopology<N, SIZE>::buf_neighbours_;
	template<unsigned N, unsigned SIZE>
	typename FlatPeriodicTopology<N, SIZE>::neighbours_type FlatPeriodicTopology<N, SIZE>::buf_neighbours_;
	template<unsigned N, unsigned SIZE>
	unsigned FlatBoundedTopology<N, SIZE>::num_neighbours;
}

#endif

