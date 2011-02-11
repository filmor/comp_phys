#ifndef TRIVIAL_PRINT_HPP
#define TRIVIAL_PRINT_HPP

#include <iostream>
#include <cassert>

namespace trivial
{

    template <typename T>
    void print (T const& val)
    {
        std::cout << val << std::endl;
    }
    
    template <typename Head, typename... Tail>
    void print (Head const& head, Tail... args)
    {
        std::cout << head << ' ';
        print(args...);
    }

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DEBUG_PRINT(expr) \
    print(__FILE__ ", " TOSTRING(__LINE__) ": " #expr " ==", expr);

}

#endif
