#include "interaction.hpp"

#include <random>
#include <boost/foreach.hpp>

namespace trivial
{

    namespace
    {
        template <typename T>
        inline void remove_element (T& vec, std::size_t index)
        {
            if (index < vec.size() - 1)
            {
                vec[index] = vec.back();
            }
            assert(index < vec.size());
            vec.pop_back();
        }
    }

    template <class P, class C, class B, class Rng>
    void world<P, C, B, Rng>::step()
    {
        updater_(particles_, clusters_, gen_);

        // Order matters here!
        for (unsigned i = 0; i < particles_.size(); ++i)
        {
            typename particle_type::interaction_type state;
            
            std::vector<std::size_t> particles_to_join;
            std::vector<std::size_t> clusters_to_join;

            // Collect particles to join with the i-th one
            for (unsigned j = 0; j < particles_.size(); ++j)
            {
                if (i == j)
                    continue;

                interact(particles_[i], particles_[j], state);

                if (state.merge(gen_))
                {
                    particles_to_join.push_back(j);
                }
            }

            // Collect clusters to join with
            for (unsigned k = 0; k < clusters_.size(); ++k)
            {
                interact(particles_[i], clusters_[k], state);
                if (state.merge(gen_))
                {
                    clusters_to_join.push_back(k);
                }
            }
            
            // If we don't merge the particle with anything we move it
            if (clusters_to_join.empty() && particles_to_join.empty())
            {
                state.move(particles_[i], gen_);
                continue;
            }

            cluster_type* cl;

            if (clusters_to_join.empty())
            {
                clusters_.push_back(cluster_type());
                cl = &clusters_.back();
            }
            else
                cl = &(clusters_[clusters_to_join[0]]);

            cl->add_particle(particles_[i]);
            remove_element(particles_, i);

            // Reverse iteration because otherwise we would move
            // clusters that still have to be merged
            BOOST_REVERSE_FOREACH( std::size_t index, clusters_to_join )
            {
                if (cl == &clusters_[index])
                    continue;
                cl->merge(clusters_[index]);
                remove_element(clusters_, index);
            }

            BOOST_REVERSE_FOREACH( std::size_t index, particles_to_join )
            {
                // If we are at the former back-element we see that it has
                // moved to particles_[i]
                std::size_t to_remove = index;
                if (index == particles_.size())
                    to_remove = i;

                cl->add_particle(particles_[to_remove]);
                remove_element(particles_, to_remove);
            }
        }

        for (unsigned i = 0; i < clusters_.size(); ++i)
        {
            typename cluster_type::interaction_type state;

            std::vector<std::size_t> clusters_to_join;

            for (unsigned j = 0; j < clusters_.size(); ++j)
            {
                // This is intended! We split clusters in updater_()
                if (i == j) continue;
                
                interact(clusters_[i], clusters_[j], state);
                if (state.merge(gen_))
                {
                    clusters_to_join.push_back(j);
                }
            }

            // first cluster never needs to move
            if (i > 0 && clusters_to_join.empty())
            {
                state.move(clusters_[i], gen_);
            }

            BOOST_REVERSE_FOREACH( std::size_t index, clusters_to_join )
            {
                clusters_[i].merge(clusters_[index]);
                remove_element(clusters_, index);
            }
        }
    }
}
