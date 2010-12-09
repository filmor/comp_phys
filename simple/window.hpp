#ifndef SIMPLE_WINDOW_HPP
#define SIMPLE_WINDOW_HPP

namespace simple
{

    class window
    {
    public:
        window (unsigned width = 800, unsigned height = 600,
                bool fullscreen = false);
        ~window();
    };

}

#endif
