#ifndef TRIVIAL_VISITOR_HPP
#define TRIVIAL_VISITOR_HPP

namespace trivial
{

	template <class Visited>
    struct visitor
	{
        virtual void visit(Visited& visited) = 0;
        virtual ~visitor() {}
	};
    
    template <class Visited>
    struct const_visitor
    {
        virtual void visit(Visited const& visited) = 0;
        virtual ~const_visitor() {}
    };

#define TRIVIAL_DEFINE_VISITABLE(type) \
    void accept (visitor<type>& v) { v.visit(*this); } \
    void accept (const_visitor<type>& v) const { v.visit(*this); }

}

#endif
