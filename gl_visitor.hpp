#ifndef TRIVIAL_GL_VISITOR_HPP
#define TRIVIAL_GL_VISITOR_HPP

#include <boost/mpl/if.hpp>

#include "visitor.hpp"
#include "vector.hpp"

#include "position.hpp"
#include "eden/particle.hpp"

namespace trivial
{
    enum { ANA_NONE = 0,
           ANA_RED = 1,
           ANA_GREEN = 2,
           ANA_BLUE = 4
    };

    namespace impl
    {
        class gl_visitor_base
        {
        protected:
            gl_visitor_base();
            ~gl_visitor_base();

            bool start_frame() { return false; }
            void end_frame();

            void handle_input();

            virtual void click_event(int x, int y) = 0;
            virtual void wheel_event(int wheel) = 0;

            static unsigned width;
            static unsigned height;
            static bool view_changed;

            unsigned tesselation_;

        private:
            double last_mouse_event_;
#if defined(_WIN32)
            static void __stdcall resize (int width, int height);
#else
            static void resize (int width, int height);
#endif
        };

        class gl_visitor_2d : public gl_visitor_base
        {
        protected:
            gl_visitor_2d();

            void update_view();

            template<class Particle>
            void draw_particle(const Particle & p, const vector<2> & offset = vector<2>())
            { draw_box(p.position + offset); }
            
            template<class Cluster>
            void draw_cluster(const Cluster & c)
            {
                draw_sphere(c.get_center(), c.get_radius());
                for(auto i = c.get_particles().begin(); i != c.get_particles().end(); ++i)
                    draw_particle(*i, c.get_cube_center());
            }

            void draw_particle(const eden::static_particle<position<2>> & p, const vector<2> & offset = vector<2>());

            bool start_frame();

            virtual void click_event(int, int);
            virtual void wheel_event(int);

        private:
            void draw_box(vector<2> const&, float r = 1, float g = 0, float b = 0);
            void draw_sphere(vector<2> const&, float);
            void set_gradient_color(float, float&, float&, float&);

            float point_size_;
            int x_, y_;
        };

        class gl_visitor_3d : public gl_visitor_base
        {
        public:
            void set_anaglyph (char left, char right)
            {
                anaglyph_ = (left & 7) + (right & 7) * 16;
            }
        protected:
            gl_visitor_3d();
            ~gl_visitor_3d();

            void update_view();
            
            template<class Particle>
            void draw_particle(const Particle & p, const vector<3> & offset = vector<3>())
            { draw_box(p.position + offset); }
            
            template<class Cluster>
            void draw_cluster(const Cluster & c)
            {
                draw_sphere(c.get_center(), c.get_radius());
                for(auto i = c.get_particles().begin(); i != c.get_particles().end(); ++i)
                    draw_particle(*i, c.get_cube_center());
            }

            bool start_frame();
            void end_frame();

            virtual void click_event(int, int);
            virtual void wheel_event(int);

        private:
            void draw_box(vector<3> const&, float r = 1, float g = 1, float b = 1);
            void draw_sphere(vector<3> const&, float);

            char anaglyph_;
            float distance_, angle_;
            int cube_list_;
            bool left_;
            void* quadric_;
        };

    }

    template <class World>
    class gl_visitor : public const_visitor<World>,
                       public boost::mpl::if_c<(World::dimension < 3),
                                               impl::gl_visitor_2d,
                                               impl::gl_visitor_3d>::type
    {
    public:
        void visit(World const& world)
        {
            this->handle_input();

            if (this->view_changed)
            {
                this->update_view();
                this->view_changed = false;
            }

            bool draw_again;
            do
            {
                draw_again = this->start_frame();

                for (auto i = world.get_particles().begin();
                    i != world.get_particles().end();
                    ++i)
                {
                    this->draw_particle(*i);
                }  
                for (auto i = world.get_clusters().begin();
                     i != world.get_clusters().end();
                     ++i)
                {
                    this->draw_cluster(*i);

                }
            }
            while (draw_again);

            this->end_frame();
        }
    };

}

#endif
