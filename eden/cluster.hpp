#ifndef TRIVIAL_EDEN_CLUSTER_HPP
#define TRIVIAL_EDEN_CLUSTER_HPP

#include <random>
#include <vector>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "../interaction.hpp"
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

        static_cluster(float eta = 1, float nm = 10240) : eta_(eta), nm_(nm) {}

        void add_particle (Particle p)
        {
            base::add_particle(p);

            if(base::particles_.size() > 1)
            {
                position_type parent;
                for(unsigned n = 0; n < 2 * position_type::dimension; ++n)
                {
                    position_type pos = p.position + (n % 2 ? 1 : -1) * get_unit_vector<position_type>(n / 2); // p still holds absolute position
                    if(has_particle_at(pos) && base::data_[pos - base::cube_center_]->score > base::data_[p.position + parent - base::cube_center_]->score)
                        parent = pos - p.position;
                }       
                base::particles_.back().parent = parent;
                base::data_[base::particles_.back().position]->parent = parent;

                add_chain_score(base::particles_.back().position, sun_score(base::particles_.back().position));

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

        void merge (static_cluster const& other) {}

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

        float standard_score(position_type & p)
        { return std::pow(1 + chain_length(p), -eta_); }

        float sun_score(position_type & p)
        {
static unsigned counter = 0;
print(counter);
            if(p[1] < 0)
                return 0;
            else if(++counter / 10000 /*% 2*/)
                return std::max(0., -(float)(p[0] + 1) / (abs(p) + 1) * (1. / chain_length(p)));
            else return std::max(0., (float)(p[0] + 1) / (abs(p) + 1) * (1. / chain_length(p)));
        }
    };
}
}

#endif
