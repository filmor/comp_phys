#ifndef TRIVIAL_HYPER_CUBE_HPP
#define TRIVIAL_HYPER_CUBE_HPP

#include <vector>

#include "vector.hpp"
#include "print.hpp"

namespace trivial
{
    namespace
    {

        template <typename T>
        T pow (T val, unsigned exponent)
        {
            T res = 1;
            for (unsigned i = 0; i < exponent; ++i)
                res *= val;
            return res;
        }
    }

    template <typename T, unsigned Dimensions>
    class hyper_cube
    {
    public:
        typedef vector<Dimensions, int> index_type;
        typedef std::vector<T> data_type;

        hyper_cube (std::size_t radius)
            : data_(pow(2 * radius + 1, Dimensions)), radius_(radius)
        {}

        std::size_t get_radius() const
        {
            return radius_;
        }

        void grow_around (index_type const& index, unsigned new_radius = 0)
        {
            if (new_radius == 0)
                new_radius = 2 * std::max(radius_, 1u);
            else
            {
                // Round up to next highest power of 2
                // http://graphics.stanford.edu/~seander/bithacks.html
                --new_radius;
                for (unsigned mult = 1; mult < sizeof(unsigned) * 8; mult *= 2)
                    new_radius |= new_radius >> mult;
                ++new_radius;
            }

            assert(new_radius > radius_);

            data_type new_data(pow(2 * new_radius + 1, Dimensions));

            // Fill new vector
            for (unsigned i = 0; i < data_.size(); ++i)
            {
                const auto new_index = get_int_index(
                                            get_vec_index(i, radius_) - index,
                                                     new_radius
                                                     );
                new_data[new_index] = data_[i];
            }

            radius_ = new_radius;
            data_.swap(new_data);
        }

        T& operator[] (index_type const& index)
        {
            return data_[get_int_index(index, radius_)];
        }

        T const& operator[] (index_type const& index) const
        {
            return data_[get_int_index(index, radius_)];
        }

    private:
        static std::size_t get_int_index (index_type const& index,
                                          std::size_t radius)
        {
            int result = 0;
            std::size_t multiplicator = 1;
            for (unsigned i = 0; i < Dimensions; ++i) 
            {
                result += multiplicator * (index[i] + radius);
                multiplicator *= 2 * radius + 1;
            }
            return result;
        }

        static index_type get_vec_index (std::size_t index, std::size_t radius)
        {
            index_type result;
            for (unsigned i = 0; i < Dimensions; ++i)
            {
                result[i] = (index % (2 * radius + 1));
                index -= result[i];
                result[i] -= radius;
                index /= 2 * radius + 1;
            }
            return result;
        }

        std::vector<T> data_; // Cube of size 2size_+1 in every dimension
        std::size_t radius_;
    };
}

#endif
