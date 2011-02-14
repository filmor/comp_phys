#ifndef TRIVIAL_STATISTICS_VISITOR_HPP
#define TRIVIAL_STATISTICS_VISITOR_HPP

#include <iostream>

#include "visitor.hpp"
#include "vector.hpp"
#include "print.hpp"

namespace trivial
{
    namespace stat
    {
        template<class World>
        struct particles
        {
            static void write_header(std::ostream & out)
            { out << "particles"; }

            static void write_value(std::ostream & out, const typename World::cluster_type & c)
            { out << c.get_size(); }
        };

        template<class World>
        struct coord_number
        {
            static void write_header(std::ostream & out)
            { out << "avg. coord. no."; }

            static void write_value(std::ostream & out, const typename World::cluster_type & c)
            {
                float coord = 0;
                for(unsigned n = 0; n < c.get_size(); ++n)
                    for(unsigned m = 0; m < 2 * World::dimension; ++m)
                        if(c.has_particle_at(c.get_particles()[n].position + (m % 2 ? 1 : -1) * get_unit_vector<typename World::position_type>(m / 2)))
                            ++coord;
                out << coord / c.get_size();
            }
        };

        template<class World, unsigned Percent>
        struct radius_of_gyration
        {
            typedef typename World::position_type::float_vector_type flt_vec_t;

            static void write_header(std::ostream & out)
            { out << "radius of gyration (" << Percent << "%)"; }

            static void write_value(std::ostream & out, const typename World::cluster_type & c)
            {
                flt_vec_t center;

                // this relies on the particles being sorted by age ascending in cluster!
                for(unsigned m = c.get_size() * (100 - Percent) / 100; m < c.get_size(); ++m)
                    center += flt_vec_t(c.get_particles()[m].position);

                center /= c.get_size();

                float rog = 0;
                for(unsigned m = 0; m < c.get_size(); ++m)
                    rog += abs2(flt_vec_t(c.get_particles()[m].position) - center);

                out << std::sqrt(rog / c.get_size());
            }
        };

        template<class World>
        struct dens_dens_correlation
        {
            typedef typename World::position_type::float_vector_type flt_vec_t;

            static void write_header(std::ostream & out)
            { out << "density density correlation"; }

            static void write_value(std::ostream & out, const typename World::cluster_type & c)
            {
                out << "{ ";
                for(unsigned d = 0; d < 2 * c.get_radius() - 1; ++d)
                    out << get_ddc(c, d) << ", ";
                out << get_ddc(c, 2 * c.get_radius() - 1) << " }";
            }

            private:
                static float get_ddc(const typename World::cluster_type & c, unsigned dist)
                {
                    float d = 0;
                    for(unsigned n = 0; n < c.get_size(); ++n)  // only sum over occupied sites (other contribute 0)
                        for(unsigned m = 0; m < 2 * World::dimension; ++m)  // average over all directions
                            if(c.has_particle_at(c.get_particles()[n].position + dist * (m % 2 ? 1 : -1) * get_unit_vector<typename World::position_type>(m / 2)))
                                ++d;
                    return d / (2 * World::dimension * c.get_size());
                }
        };
    }

    template<class ... Any> //might become unneccessary with the advance of c++0x
    class statistics_visitor;
    
    template<typename World, typename Head, typename ... Tail>
    class statistics_visitor<World, Head, Tail...> : public statistics_visitor<World, Tail...>
    {
        typedef statistics_visitor<World, Tail...> base;

        public:
            statistics_visitor(std::ostream & out) : base(out) {}

            void visit(const World & world)
            {
                if(!base::header_)
                {
                    base::out_ << "# Step\tCluster\t";
                    write_header();
                    base::header_ = true;
                }

                for(unsigned n = 0; n < world.get_clusters().size(); ++n)
                {
                    base::out_ << base::step_ << "\t" << n << "\t";
                    write_values(world.get_clusters()[n]);
                }

                ++base::step_;
            }

        protected:
            void write_header()
            {
                Head::write_header(base::out_);
                base::out_ << "\t";
                base::write_header();
            }

            void write_values(const typename World::cluster_type & c)
            {
                Head::write_value(base::out_, c);
                base::out_ << "\t";
                base::write_values(c);
            }
    };

    template<class World>
    class statistics_visitor<World> : public const_visitor<World>
    {
        public:
            statistics_visitor(std::ostream & out) : out_(out), header_(false), step_(1) {}

        protected:
            void write_header()
            { out_ << std::endl; }

            void write_values(const typename World::cluster_type & c)
            { out_ << std::endl; }

            std::ostream & out_;
            bool header_;
            unsigned step_;
    };
}

#endif
