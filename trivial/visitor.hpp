#ifndef TRIVIAL_VISITOR_HPP
#define TRIVIAL_VISITOR_HPP

#include <GL/glfw.h>
#include <iostream>

namespace trivial
{
	using std::cout;
	using std::endl;

	template<class TOPOLOGY>
	class Particle;
	template<class TOPOLOGY>
	class Cluster;

	template<class VISITED>
	class Visitor
	{
		public:
			virtual void visit(VISITED * visisted) = 0;
			virtual ~Visitor() {}
	};

	template<class TOPOLOGY, class WORLD>
	class OmniVisitor : Visitor<Particle<TOPOLOGY>>, Visitor<Cluster<TOPOLOGY>>, Visitor<WORLD>
	{
		public:
			virtual ~OmniVisitor() {}
	};

}

#endif
