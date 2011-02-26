#ifndef TRIVIAL_TGA_VISITOR_HPP
#define TRIVIAL_TGA_VISITOR_HPP

#include <fstream>

#include "visitor.hpp"
#include "eden.hpp"

namespace trivial
{
    template<class World>
    class tga_visitor : public const_visitor<World>
    {
        const char * filename_;

        template<class Particle>
        void set_color(const Particle & p, char * color)
        {
            color[0] = 0; color[1] = 0; color[2] = 255;
        }

        void set_color(const eden::static_particle<position<2>> & p, char * color) //TODO doubles code of gl_visitor_2d
        {
            const float a1 = 0.16f, a2 = 0.33f, a3 = 0.50f, a4 = 0.66f, a5 = 0.83f, a6 = 1.00f;
            const float value = p.score;
            float r, g, b;

            if(value < a2)
            { r = 0; g = std::max(0.0f, (value - a1)/(a2 - a1)); b = 1; }
            else if(value < a3)
            { r = 0; g = 1; b = std::cos((a2 - value)/(a3 - a2) * M_PI * 0.5f); }
            else if(value < a4)
            { r = std::sin((value - a3)/(a4 - a3) * M_PI * 0.5f); g = 1; b = 0; }
            else if(value < a5)
            { r = 1; g = std::max(0.0f, (a5 - value)/(a5 - a4)); b = 0; }
            else
            { r = 1; g = 0; b = std::min(0.8f, (value - a5)/(a6 - a5)); }
    
            color[0] = b * 255;
            color[1] = g * 255;
            color[2] = r * 255;
        }

        void visit(World const& world)
        {
            assert(World::dimension == 2);

            int xmin = 0, ymin = 0, xmax = 0, ymax = 0;

            for (unsigned n = 0; n < world.get_clusters().size(); ++n)
            {
                xmin = std::min(xmin, (int)std::floor(world.get_clusters()[n].get_center()[0] - world.get_clusters()[n].get_radius()));
                xmax = std::max(xmax, (int)std::ceil(world.get_clusters()[n].get_center()[0] + world.get_clusters()[n].get_radius()));
                ymin = std::min(xmin, (int)std::floor(world.get_clusters()[n].get_center()[1] - world.get_clusters()[n].get_radius()));
                ymax = std::max(xmax, (int)std::ceil(world.get_clusters()[n].get_center()[1] + world.get_clusters()[n].get_radius()));
            }

            char header[] = { 0, // id length 0
                    0, // no palette
                    2, // rgb
                    0, 0, // palette start
                    0, 0, // palette length
                    16, // palette entry size
                    0, 0, // x origin
                    0, 0, // y origin
                    (char)((xmax - xmin + 1) % 256), (char)((xmax - xmin + 1) / 256), // width
                    (char)((ymax - ymin + 1) % 256), (char)((ymax - ymin + 1) / 256), // height
                    24, // bpp
                    0 // no attribute-bits, origin bottom left
                };
            
            std::ofstream o(filename_, std::ofstream::binary);
            o.write(header, sizeof(header));

            long data_start = o.tellp();

            char color[] = { (char)192, (char)192, (char)192 };
            for(int x = xmin; x <= xmax; ++x)
                for(int y = ymin; y <= ymax; ++y)
                    o.write(color, sizeof(color));

            for(unsigned n = 0; n < world.get_clusters().size(); ++n)
                for(unsigned m = 0; m < world.get_clusters()[n].get_size(); ++m)
                {
                    const typename World::particle_type & p = world.get_clusters()[n].get_particles()[m];
                    set_color(p, color);
                    o.seekp(data_start + sizeof(color) * ((xmax - xmin + 1) * (p.position[1] - ymin) + (p.position[0] - xmin)));
                    o.write(color, sizeof(color));
                }

            o.close();
        }

        public:
            tga_visitor(const char * filename) : filename_(filename) {};
    };
}

#endif
