#include "lattice.hpp"
#include "cluster.hpp"
#include "random_walker.hpp"
#include "random_node.hpp"

#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/random.hpp>

using namespace simple;
using namespace boost;

int main()
{
    /// Threadsafe machen!
    mt19937 gen (std::time(0));

    const unsigned N = 1e4; // Wollen einen Cluster der Größe 10.000

    typedef infinite_rectangular_lattice lattice_type;
    typedef infinite_rectangular_lattice::node_type node_type;
    typedef cluster<infinite_rectangular_lattice> cluster_type;

    // Seed
    cluster_type cl = { node_type() };

    random_walker<lattice_type, mt19937&> w (gen);

    for (unsigned k = 1; k < N; ++k)
    {
        w.set_node(random_node<lattice_type>(cl.center(), k, gen));

        if (cl.contains(w.get_node()))
        {
            --k;
            continue;
        }

        for(;;)
        {
            unsigned d = static_cast<unsigned> (std::sqrt(distance2(cl, w.get_node())));

            // TODO: < EPS, nicht gleich 0
            if (d == 0)
                break;
            else
                w.advance(d);
        }

        cl.add_node(w.get_node());

        std::cout << k << std::endl;

    }

}

