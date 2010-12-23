#ifndef SIMPLE_LATTICE_HPP
#define SIMPLE_LATTICE_HPP

#include <iostream>
#include <cmath>

#include "vector.hpp"

namespace simple
{

    template <unsigned Directions, unsigned Dimensions>
    struct discrete_lattice
    {
        static const unsigned directions = Directions;
        static const unsigned dimensions = Dimensions;
    };

    inline int abs(int k) { return k > 0 ? k : -k; }

    /// Beispielimplementierung
    class infinite_rectangular_lattice : public discrete_lattice<4, 2>
    {
    public:
        struct node_type;
    };

    struct infinite_rectangular_lattice::node_type
    {
        node_type () { x = 0; y = 0; }
        node_type (node_type const& n)
        {
            x = n.x;
            y = n.y;
        }

        node_type (vector<2> const& v)
        {
            x = int(v[0] + 0.5);
            y = int(v[1] + 0.5);
        }

        vector<2>&& get_vector () const
        {
            return std::move(vector<2> {float(x), float(y)});
        }

        friend unsigned dist(node_type const& n1, node_type const& n2)
        {
            return abs(n1.x - n2.x) + abs(n1.y - n2.y);
        }

        void step(unsigned direction, unsigned step_size = 1)
        {
            switch (direction)
            {
            case 1:
                x += step_size;
                break;
            case 2:
                y += step_size;
                break;
            case 3:
                x -= step_size;
                break;
            case 4:
                y -= step_size;
                break;
            }
        }
        
        bool operator== (const node_type& other) const
        {
            return other.x == x && other.y == y;
        }

        int x;
        int y;
    };


    std::ostream& operator<< (std::ostream& out,
                              infinite_rectangular_lattice::node_type const& n)
    {
        out << '(' << n.x << ',' << n.y << ')';
        return out;
    }

}

#endif
