#include "vector.hpp"
#include "position.hpp"

using namespace trivial;

int main ()
{
    vector<5> bla;
    vector<5, float> blubb;

    bla + bla;
    bla - bla;
    vector<5, float> (bla) - blubb;

    position<5> p;

    p + p;

    p + bla;

}
