#ifndef TRIVIAL_HYPER_CUBE_HPP
#define TRIVIAL_HYPER_CUBE_HPP

#include "vector.hpp"

#include "print.hpp"

#include <vector>
#include <cmath>
#include <stdexcept>

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
            : data_(pow(2 * size + 1, Dimensions)), size_(size)
        {}

        std::size_t get_radius() const
        {
            return size_;
        }

        void grow ()
        {
            // TODO: Assertions!
            std::size_t new_size = 2 * size_;
            data_type new_data(pow(2 * new_size + 1, Dimensions));

            // Fill new vector
            for (unsigned i = 0; i < data_.size(); ++i)
            {
                auto new_index = get_int_index(get_vec_index(i, size_), new_size);
                // print("moving object from", i, "to", new_index);
                new_data.at(new_index) = data_[i];
            }

            // print("size", size_);
            // print("new_size", new_size);
            size_ = new_size;
            data_.swap(new_data);
        }

        T& operator[] (index_type const& index)
        {
            for (unsigned i = 0; i < index.size(); ++i)
                if (unsigned(std::abs(index[i])) > size_)
                    throw std::out_of_range("");
            return data_[get_int_index(index, size_)];
        }

        T const& operator[] (index_type const& index) const
        {
            for (unsigned i = 0; i < index.size(); ++i)
                if (unsigned(std::abs(index[i])) > size_)
                    throw std::out_of_range("");
            return data_[get_int_index(index, size_)];
        }

    private:
        static
            std::size_t get_int_index (index_type const& index,
                                       std::size_t size)
        {
            int result = 0;
            std::size_t multiplicator = 1;
            for (unsigned i = 0; i < Dimensions; ++i) 
            {
                result += multiplicator * (index[i] + size);
                multiplicator *= 2 * size + 1;
            }
            //print("index:", index, "int_index:", result,
            //      "get_vec_index:",
            //      get_vec_index(result, size));
            return result;
        }

        static index_type get_vec_index (std::size_t index, std::size_t size)
        {
            index_type result;
            for (unsigned i = 0; i < Dimensions && index; ++i)
            {
                result[i] = index % (2 * size + 1) - size;
                index /= 2 * size + 1;
            }
            return result;
        }

        std::vector<T> data_; // Cube of size 2size_+1 in every dimension
        std::size_t size_;
    };
}

#endif
