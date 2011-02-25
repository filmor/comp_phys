#ifndef TRIVIAL_BATH_HPP
#define TRIVIAL_BATH_HPP

#include <vector>
#include <random>

#include "../print.hpp"

namespace trivial
{
namespace meakin
{

    template <class Particle, class Cluster, int Size, unsigned FreeParticles>
    class uniform_updater
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        template<class RandomNumberGenerator>
        void operator()(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters, RandomNumberGenerator& rng) 
        {
            for (unsigned n = particles.size(); n < FreeParticles; ++n)
            {
                position_type pos;
                for (bool collision = true; collision;)
                {
                    for (unsigned m = 0; m < position_type::dimension; ++m)
                        pos += get_unit_vector<position_type>(m) 
                                * ((float)rng() / rng.max() * Size - Size / 2);

                    collision = false;

                    for (unsigned m = 0; m < particles.size(); ++m)
                        if (particles[m].position == pos)
                        {
                            collision = true;
                            break;
                        }
                    
                    if (!collision)
                        for (unsigned m = 0; m < clusters.size(); ++m)
                            if (clusters[m].has_particle_at(pos))
                            {
                                collision = true;
                                break;
                            }
                }
                particles.push_back(Particle(pos));
            }

        }
    };

    template <class Particle, class Cluster, unsigned FreeParticles>
    class diffusion_limited_updater
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        diffusion_limited_updater() : seeded_(false) {}

        template<class RandomNumberGenerator>
        void operator()(std::vector<particle_type>& particles,
                        std::vector<cluster_type>& clusters,
                        RandomNumberGenerator& rng) 
        {
            if(!seeded_)
            {
                position_type p;
                clusters.push_back(cluster_type());
                clusters.back().add_particle(particle_type(p));
                p += get_unit_vector<position_type>(0);
                clusters.back().add_particle(particle_type(p));
                seeded_ = true;
            }

            const position_type& center = clusters[0].get_center();
            const float radius = clusters[0].get_radius();

            int create = FreeParticles;
            for(unsigned n = 0; n < particles.size(); ++n)
                if(abs(particles[n].position - center)
                        > std::max(2 * radius, radius + 10))
                {
                    particles.erase(particles.begin() + n);
                    --n;
                }
                else
                {
                    --create;
                }
        
            while(create-- > 0) 
            {
                position_type pos;
                for(bool collision = true; collision;)
                {
                    typename position_type::float_vector_type e;
                    float mod = 0;
                    for(unsigned m = 0; m < e.size(); ++m)
                    {
                        e[m] = norm_dist_(rng);
                        mod += e[m] * e[m];
                    }
                    mod = sqrt(mod);
                    for(unsigned m = 0; m < position_type::dimension; ++m)
                        pos += get_unit_vector<position_type>(m)
                             * (e[m] * (radius + 5) / mod);
                    pos += center;

                    collision = false;
                    for(unsigned m = 0; m < particles.size(); ++m)
                        if(particles[m].position == pos)
                        {
                            collision = true;
                            break;
                        }
                    
                    if(!collision)
                        for(unsigned m = 0; m < clusters.size(); ++m)
                            if(clusters[m].has_particle_at(pos))
                            {
                                collision = true;
                                break;
                            }
                }
                particles.push_back(particle_type(pos));
            }

        }

    private:
        std::normal_distribution<float> norm_dist_;
        bool seeded_;
    };

    template<class Particle, class Cluster, int Size, unsigned Particles>
    class static_updater : public uniform_updater<Particle, Cluster, Size, Particles>
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        static_updater() : done_(false) {}

        template<class RandomNumberGenerator>
        void operator()(std::vector<particle_type>& particles,
                        std::vector<cluster_type> & clusters,
                        RandomNumberGenerator & rng) 
        {
            if(done_)
                return;
            uniform_updater<Particle, Cluster, Size, Particles>::operator()
                (particles, clusters, rng);
            done_ = true;
        }

    private:
        bool done_;
    };

    template<class Particle, class Cluster, class World, unsigned Particles>
    class cluster_updater
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        cluster_updater() {}

        template<class RandomNumberGenerator>
        void operator()(std::vector<particle_type>& particles,
                        std::vector<cluster_type> & clusters,
                        RandomNumberGenerator & rng) 
        {
            assert(clusters.size() <= 2);

            if(clusters.size() == 0)
                clusters.push_back(create_cluster());

            if(clusters.size() == 2)
            {
                const position_type& center = clusters[0].get_center(), center_new = clusters[1].get_center();
                const float radius = clusters[0].get_radius(), radius_new = clusters[1].get_radius();

                if(abs(center - center_new) > std::max(2 * (radius + radius_new), radius + radius_new + 10))                
                    move_to_border(clusters[0], clusters[1], rng);

                return;
            }

            Cluster cluster = create_cluster();
            move_to_border(clusters[0], cluster, rng);
            clusters.push_back(cluster);
        }

    private:
        std::normal_distribution<float> norm_dist_;

        Cluster create_cluster()
        {
            World w;
            do{ w.step(); }
            while(w.get_clusters().back().get_size() < Particles);
            return Cluster(w.get_clusters().back());
        }

        template<class RandomNumberGenerator>
        void move_to_border(const Cluster & c0, Cluster & c1, RandomNumberGenerator & rng)
        {
            position_type pos;
            typename position_type::float_vector_type e;
            float mod = 0;
            for(unsigned m = 0; m < e.size(); ++m)
            {
                e[m] = norm_dist_(rng);
                mod += e[m] * e[m];
            }
            mod = sqrt(mod);
            for(unsigned m = 0; m < position_type::dimension; ++m)
                pos += get_unit_vector<position_type>(m)
                     * (e[m] * (c0.get_radius() + c1.get_radius() + 5) / mod);

            c1.move(pos + c0.get_center() - c1.get_center());
        }
    };

}
}

#endif

