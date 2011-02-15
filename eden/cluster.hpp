#ifndef TRIVIAL_EDEN_CLUSTER_HPP
#define TRIVIAL_EDEN_CLUSTER_HPP

#include <random>
#include <vector>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "../meakin/cluster.hpp"

namespace trivial
{
namespace eden
{
    template <class Particle>
    class static_cluster : public meakin::static_cluster<Particle>
    {
    public:
        typedef Particle particle_type;
        typedef typename Particle::position_type position_type;
        typedef meakin::static_cluster<Particle> base;

        static_cluster(float eta = 1, float nm = 102400) : eta_(eta), nm_(nm) {}

        template<class RandomNumberGenerator> 
        void add_particle (Particle p, RandomNumberGenerator & rng)
        {
            base::add_particle(p, rng);

            if(base::particles_.size() > 1)
            {
                std::vector<unsigned> neighbours;
                for(unsigned n = 0; n < 2 * position_type::dimension; ++n)
                    if(has_particle_at(p.position + (n % 2 ? 1 : -1) * get_unit_vector<position_type>(n / 2))) // p still holds absolute position
                        neighbours.push_back(n);
                unsigned n = neighbours[std::uniform_int_distribution<unsigned>(0, neighbours.size() - 1)(rng)];
                base::particles_.back().parent = (n % 2 ? 1 : -1) * get_unit_vector<position_type>(n / 2);
                base::data_[base::particles_.back().position]->parent = base::particles_.back().parent;

                add_chain_score(base::particles_.back().position, std::pow(1 + chain_length(base::particles_.back().position), -eta_));

                for(int m = base::particles_.size() - 1; m >= 0; --m)
                {
                    base::particles_[m].score -= 1 / nm_;
                    base::data_[base::particles_[m].position]->score -= 1 / nm_;

                    if(base::particles_[m].score < 0)
                    {
                        base::data_[base::particles_[m].position] = boost::optional<Particle>();
                        if(m < base::particles_.size() - 1)
                            base::particles_[m] = base::particles_.back();
                        base::particles_.pop_back();
                    }
                }
            }
        } 

        template<class RandomNumberGenerator>
        void merge (static_cluster const& other, RandomNumberGenerator & rng)
        {
            throw "eden cluster may not be merged";
        }

    private:
        float eta_, nm_;

        int chain_length(const position_type & p)
        {
            if(abs_inf(base::data_[p]->parent) == 0)   // this is the origin
                return 1;
            return chain_length(p + base::data_[p]->parent) + 1;
        }

        void add_chain_score(const position_type & p, float score)
        {
            base::data_[p]->score += score;

            BOOST_FOREACH(Particle & p2, base::particles_)    // TODO: bad
            {
                if(p2.position == p)
                {
                    p2.score += score;
                    break;
                }
            }

            if(abs_inf(base::data_[p]->parent) > 0)   // this is not yet the origin
                add_chain_score(p + base::data_[p]->parent, score);
        }
    };
}
}

#endif
