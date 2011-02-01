#ifndef TRIVIAL_HYPER_CUBE_HPP
#define TRIVIAL_HYPER_CUBE_HPP

#include "vector.hpp"

#include <vector>

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

        hyper_cube (std::size_t size)
            : data_(pow(size, Dimensions)), size_(size)
        {}

        void grow_around (index_type const& center)
        {
            // Assertion!
            std::size_t new_size = 2 * size_;
            data_type new_data(pow(2 * new_size + 1, Dimensions));

            // Fill new vector
            for (unsigned i = 0; i < data_.size(); ++i)
            {
                new_data.at(
                        get_int_index((get_vec_index(i, size_) - center)
                                      , new_size))
                    = data_[i];
            }

            size_ = new_size;
            data_.swap(new_data);
        }

        T& operator[] (index_type const& index)
        {
            return data_[get_int_index(index, size_)];
        }

        T const& operator[] (index_type const& index) const
        {
            return data_[get_int_index(index, size_)];
        }

    private:
        static
            std::size_t get_int_index (index_type const& index,
                                       std::size_t size)
        {
            std::size_t result = 0;
            std::size_t multiplicator = 1;
            for (unsigned i = 0; i < Dimensions; ++i, multiplicator *= size)
                result += multiplicator * (index[i] + size);
            return result;
        }

        static index_type get_vec_index (std::size_t index, std::size_t size)
        {
            index_type result;
            for (unsigned i = 0; i < Dimensions && index; ++i)
            {
                result[i] = index % size;
                index /= size;
            }
            return result;
        }

        std::vector<T> data_; // Cube of size 2size_+1 in every direction
        std::size_t size_;
    };
}

#endif
