#ifndef SIMPLE_CLUSTER_HPP
#define SIMPLE_CLUSTER_HPP

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>

namespace simple
{

    template <typename Lattice>
    class cluster
    {
    public:
        typedef typename Lattice::node_type node_type;

        cluster(std::initializer_list<node_type> nodes)
            : nodes_(nodes)
        {}

        inline friend
        float distance2 (node_type const& node, cluster const& cl)
        {
            return distance2(cl, node);
        }

        inline friend
        float distance2 (cluster const& cl, node_type const& node)
        {
            float res = 0.0;

            BOOST_FOREACH(node_type const& i, cl.nodes_)
            {
                res = std::min(distance2(i, node), res);
            }

            return res;
        }

        void add_node(node_type node)
        {
            nodes_.push_back(node);
        }

        node_type center () const
        {
            if (nodes_.size() > 0)
                return nodes_[0];
            else
                return node_type();
        }

        bool contains (node_type const& n)
        {
            auto end = nodes_.end();
            return end != std::find(nodes_.begin(), end, n);
        }

    private:
        std::vector<node_type> nodes_;
    };

}

#endif
