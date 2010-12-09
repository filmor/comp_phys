#include "lattice.hpp"
#include "cluster.hpp"
#include "random_walker.hpp"
#include "random_node.hpp"
#include "window.hpp"

#include <GL/glfw.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/random.hpp>

using namespace simple;
using namespace boost;

typedef rand48 generator_type;

int main()
{
    /// Threadsafe machen!
    generator_type gen (std::time(0));

    const unsigned N = 1e3; // Wollen einen Cluster der Größe 10.000

    typedef infinite_rectangular_lattice lattice_type;
    typedef infinite_rectangular_lattice::node_type node_type;
    typedef cluster<infinite_rectangular_lattice> cluster_type;

    // Seed
    cluster_type cl = { node_type() };

    random_walker<lattice_type, generator_type&> w (gen);

    window win;

    for (unsigned k = 1; k < N; ++k)
    {
        w.set_node(random_node<lattice_type>(cl.center(), k, gen));

        if (cl.contains(w.get_node()))
        {
            --k;
            continue;
        }

        unsigned starting_distance 
            = distance(cl, w.get_node());

        bool found_one = false;

        for(;;)
        {
            unsigned d = distance(cl, w.get_node());

            if (d < 2)
            {
                found_one = true;
                break;
            }
            else if (starting_distance < 1.5 * d)
                break;
            else
                w.advance(d > 9.f ? std::sqrt(d) : 1.f);
        }
        if (found_one)
            cl.add_node(w.get_node());
        else
        {
            --k; continue;
        }

        /// Drawing
        if (k % 10 == 0)
        {
            glClear (GL_COLOR_BUFFER_BIT);
            glBegin (GL_QUADS);

            for (int i = 0; i < cl.get_nodes().size(); ++i)
            {
                node_type const& node = cl.get_nodes()[i];
                const unsigned x = node.x + 200;
                const unsigned y = node.y + 50;

                glVertex2i (2 * x,       2 * y);
                glVertex2i (2 * (x + 1), 2 * y);
                glVertex2i (2 * (x + 1), 2 * (y + 1));
                glVertex2i (2 * x,       2 * (y + 1));
            }

            glEnd ();

            glfwSwapBuffers ();
            std::cout << k << std::endl;
        }
    }

}

