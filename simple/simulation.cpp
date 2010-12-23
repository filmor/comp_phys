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

    const unsigned N = 1e4; // Wollen einen Cluster der Größe 10.000

    typedef infinite_rectangular_lattice lattice_type;
    typedef infinite_rectangular_lattice::node_type node_type;
    typedef cluster<infinite_rectangular_lattice> cluster_type;

    // Seed
    cluster_type cl = { node_type() };

    random_walker<lattice_type, generator_type&> w (gen);

    window win;

    for (unsigned k = 1; k < N; ++k)
    {
        const float radius = cl.radius();
        w.set_node(random_node<lattice_type>(cl.center(),
                                             std::max(2.0f, 1.5f * radius), gen));

        if (cl.contains(w.get_node()))
        {
            --k;
            continue;
        }

        bool found_one = false;

        for(; !found_one; )
        {
            /// Erst Distanz zum Zentrum messen
            unsigned d = dist(cl, w.get_node());

            if (d < 2)
                found_one = true;
            else if (d > 2.0 * radius)
                break;
            else
                w.step(); // w.advance(d > 9.f ? std::sqrt(d) : 1.f);
        }
        if (found_one)
            cl.add_node(w.get_node());
        else
        {
            --k; continue;
        }

        /// Drawing
        glClear (GL_COLOR_BUFFER_BIT);
        glBegin (GL_QUADS);

        for (unsigned i = 0; i < cl.get_nodes().size(); ++i)
        {
            node_type const& node = cl.get_nodes()[i];
            const unsigned x = node.x + 200;
            const unsigned y = node.y + 150;

            glVertex2i (2 * x,       2 * y);
            glVertex2i (2 * (x + 1), 2 * y);
            glVertex2i (2 * (x + 1), 2 * (y + 1));
            glVertex2i (2 * x,       2 * (y + 1));
        }

        glEnd ();

        glfwSwapBuffers ();
        std::cout << k << ": " << cl.radius() << ' ' << cl.center() << std::endl;
    }

}

