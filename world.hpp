#ifndef TRIVIAL_WOLRD_HPP
#define TRIVIAL_WORLD_HPP

#include <vector>
#include <random>
#include <boost/foreach.hpp>

#include "position.hpp"
#include "cluster.hpp"
#include "bath.hpp"
#include "visitor.hpp"

namespace trivial
{
	template <class Particle, class Bath,
              class RandomNumberGenerator = std::mt19937>
	class world
	{
    public:
        typedef Particle                            particle_type;
        typedef typename Particle::position_type    position_type;
        typedef typename position_type::vector_type vector_type;
        typedef cluster<particle_type>              cluster_type;

        world(RandomNumberGenerator rng = RandomNumberGenerator())
            : gen_(rng)
        {}

        void add_cluster(cluster_type const& cl)
        {
            // TODO: Collision checking!
            clusters_.push_back(cl);
        }

        void step ();

        void accept (world_visitor<world>& v)
        {
            for(unsigned n = 0; n < particles_.size(); ++n)
                particles_[n].accept(
                    static_cast<visitor<particle_type>&> (v)
                    );
            for(unsigned n = 0; n < clusters_.size(); ++n)
                clusters_[n].accept(
                    static_cast<visitor<cluster_type>&> (v));
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

    private:
        std::vector<particle_type> particles_;
        std::vector<cluster_type> clusters_;
        Bath bath_;
        RandomNumberGenerator gen_;
	};
}

#include "world.ipp"
#endif

