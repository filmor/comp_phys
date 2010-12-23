#ifndef SIMPLE_CLUSTER_HPP
#define SIMPLE_CLUSTER_HPP

#include "vector.hpp"

#include <vector>
#include <boost/foreach.hpp>

namespace simple
{

    template <typename Lattice>
    class cluster
    {
    public:
        typedef typename Lattice::node_type node_type;
        typedef vector<Lattice::dimensions> vec_type;

        cluster(std::initializer_list<node_type> nodes)
            : radius_(0.0), nodes_(nodes)
        {}

        inline friend
        float dist (node_type const& node, cluster const& cl)
        {
            return dist(cl, node);
        }

        inline friend
        float dist (cluster const& cl, node_type const& node)
        {
            unsigned res = dist(cl.center(), node);

            BOOST_FOREACH(node_type const& i, cl.nodes_)
            {
                res = std::min(dist(i, node), res);
            }

            return res;
        }

        void add_node(node_type node)
        {
            vec_type vec = node.get_vector();
            for (unsigned i = 0; i < Lattice::dimensions; ++i)
                sum_[i] += vec[i];

            radius_ = std::max(dist(vec, center_vector()), radius_);

            nodes_.push_back(node);
        }

        float radius () const
        {
            return radius_;
        }

        node_type center () const
        {
            return node_type(center_vector());
        }

        vec_type center_vector () const
        {
            return sum_ / float(nodes_.size());
        }

        // TODO: Das geht geschickter!
        bool contains (node_type const& n)
        {
            for (unsigned i = 0; i < nodes_.size(); ++i)
            {
                if (n == nodes_[i])
                    return true;
            }
            return false;
        }

        std::vector<node_type> const& get_nodes() const { return nodes_; }

    private:
        float radius_;
        vector<Lattice::dimensions> sum_;
        std::vector<node_type> nodes_;
    };

}

#endif
