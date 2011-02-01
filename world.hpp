#ifndef TRIVIAL_WOLRD_HPP
#define TRIVIAL_WORLD_HPP

#include <vector>

#include <boost/foreach.hpp>

#include "position.hpp"
#include "particle.hpp"
#include "cluster.hpp"
#include "bath.hpp"
#include "visitor.hpp"

namespace trivial
{
	template<class Particle, class Bath>
	class world
	{
    public:
        typedef Particle                            particle_type;
        typedef typename Particle::position_type    position_type;
        typedef cluster<particle_type>              cluster_type;

        void add_cluster(cluster_type const& cl)
        {
            clusters_.push_back(cl);
        }

        void step()
        {
            bath_.step(particles_, clusters_);

            // TODO: Probability matrix for the particle!
    
            // Order matters here!
            for (unsigned i = 0; i < particles_.size(); ++i)
            {
                std::vector<std::size_t> particles_to_join;
                std::vector<std::size_t> clusters_to_join;

                // interact is symmetric!
                for (unsigned j = 0; j < particles_.size(); ++j)
                {
                    if (i == j) continue;

                    auto result = interact(particles_[i], particles_[j]);
                    if (result == particle_type::RESULT_JOIN)
                    {
                        particles_to_join.push_back(j);
                    }
                }

                for (unsigned k = 0; k < clusters_.size(); ++k)
                {
                    auto result = interact(particles_[i], clusters_[k]);
                    if (result == particle_type::RESULT_JOIN)
                    {
                        clusters_to_join.push_back(k);
                    }
                }

                if (clusters_to_join.empty() && particles_to_join.empty())
                {
                    particles_[i].move();
                    continue;
                }

                cluster_type* cl
                    = clusters_to_join.empty() ?
                        new cluster_type
                      : &(clusters_[clusters_to_join[0]]);

                // Reverse iteration because otherwise we would move
                // clusters that still have to be merged
                BOOST_REVERSE_FOREACH( std::size_t index, clusters_to_join )
                {
                    cl->merge(clusters_[index]);
                    clusters_[index] = clusters_.back();
                    clusters_.pop_back();
                }

                bool added_this_element = false;
                BOOST_REVERSE_FOREACH( std::size_t index, particles_to_join )
                {
                    if (index < i && !added_this_element)
                    {
                        cl->add_particle(particles_[i]);
                        particles_[i] = particles_.back();
                        particles_.pop_back();
                    }

                    cl->add_particle(particles_[index]);
                    particles_[index] = particles_.back();
                    particles_.pop_back();
                }

                if (clusters_to_join.empty())
                {
                    clusters_.push_back(*cl);
                    delete cl;
                }
            }

            for (unsigned i = 0; i < clusters_.size(); ++i)
            {
                std::vector<std::size_t> clusters_to_join;

                for (unsigned j = 0; j < clusters_.size(); ++j)
                {
                    if (i == j) continue;
                    auto result = interact(clusters_[i], clusters_[j]);
                    if (result == particle_type::RESULT_JOIN)
                    {
                        clusters_to_join.push_back(j);
                    }
                }                    

                BOOST_REVERSE_FOREACH( std::size_t index, clusters_to_join )
                {
                    clusters_[i].merge(clusters_[index]);
                    clusters_[index] = clusters_.back();
                    clusters_.pop_back();
                }

                clusters_[i].move();
            }
        }

        void accept (world_visitor<world>& v)
        {
            for(unsigned n = 0; n < particles_.size(); ++n)
                particles_[n].accept(v);
            for(unsigned n = 0; n < clusters_.size(); ++n)
                clusters_[n].accept(v);
            ((visitor<world> &)v).visit(*this);
        }

        void accept (const_world_visitor<world>& v) const
        {
            for (unsigned n = 0; n < particles_.size(); ++n)
                particles_[n].accept(
                    static_cast<const_visitor<particle_type>&> (v)
                    );

            for (unsigned n = 0; n < clusters_.size(); ++n)
                clusters_[n].accept(
                    static_cast<const_visitor<cluster_type>&> (v)
                    );
            
            static_cast<const_visitor<world>&> (v).visit(*this);
        }

        std::vector<cluster_type> const& get_clusters() const
        {
            return clusters_;
        }

        std::vector<particle_type> const& get_particles() const
        {
            return particles_;
        }

        ~world() {}

    private:
        std::vector<particle_type> particles_;
        std::vector<cluster_type> clusters_;
        Bath bath_;
	};
}

#endif

