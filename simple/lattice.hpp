#ifndef SIMPLE_LATTICE_HPP
#define SIMPLE_LATTICE_HPP

#include <boost/mpl/bool.hpp>
#include <iostream>

#include "vector.hpp"

namespace simple
{

    template <unsigned Directions, unsigned Dimensions>
    struct discrete_lattice
    {
        static const unsigned directions = Directions;
        static const unsigned dimensions = Dimensions;
    };

    /// Beispielimplementierung
    class infinite_rectangular_lattice : public discrete_lattice<4, 2>
    {
    public:
        struct node_type
        {
            node_type () { x = 0; y = 0; }
            node_type (node_type const& n)
            {
                x = n.x;
                y = n.y;
            }

            /// Setze x und y so, dass der Knoten nahe bei v liegt
            void set_near (vector<2> const& v)
            {
                x = int(v[0] + 0.5);
                y = int(v[1] + 0.5);
            }

            vector<2>&& get_vector () const
            {
                return std::move(vector<2> {float(x), float(y)});
            }

            friend float distance2(node_type const& n1, node_type const& n2)
            {
                const int dx = n1.x - n2.x;
                const int dy = n1.y - n2.y;
                return dx * dx + dy * dy;
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
    };

    std::ostream& operator<< (std::ostream& out,
                              infinite_rectangular_lattice::node_type const& n)
    {
        out << '(' << n.x << ',' << n.y << ')';
        return out;
    }

}

#endif
