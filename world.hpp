#ifndef TRIVIAL_WOLRD_HPP
#define TRIVIAL_WORLD_HPP

#include <vector>
#include <random>

#include "visitor.hpp"

namespace trivial
{
    template <class Particle, class Cluster, class Updater,
              class RandomNumberGenerator = std::mt19937>
    class world
    {
    public:
        typedef Particle                            particle_type;
        typedef typename Particle::position_type    position_type;
        typedef typename position_type::vector_type vector_type;
        typedef Cluster                             cluster_type;
        static const unsigned dimension = position_type::dimension;

        world(RandomNumberGenerator rng = RandomNumberGenerator())
            : gen_(rng)
        {}

        void add_cluster(cluster_type const& cl)
        {
            // TODO: Collision checking!
            clusters_.push_back(cl);
        }

        void add_particle(particle_type const& p)
        {
            particles_.push_back(p);
        }

        inline void step ();

        TRIVIAL_DEFINE_VISITABLE(world)

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
        Updater updater_;
        RandomNumberGenerator gen_;
    };
}

#include "world.ipp"
#endif

