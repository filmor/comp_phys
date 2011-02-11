#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <boost/format.hpp>

using namespace std;
using boost::format;

void gradient(float value, char * data)
{
    float sq0 = 0.00f;
    float sq1 = 0.16f;
    float sq2 = 0.33f;
    float sq3 = 0.50f;
    float sq4 = 0.66f;
    float sq5 = 0.83f;
    float sq6 = 1.00f;

    if(value < sq2)
    {
        data[0] = 0;
        data[1] = (char)(255 * max(0.0f, (value - sq1)/(sq2 - sq1)));
        data[2] = 255;                               
    }
    else if(value < sq3)
    {
        data[0] = 0;
        data[1] = 255;
        data[2] = (char)(255 * cos((sq2 - value)/(sq3 - sq2) * M_PI * 0.5f));
    }
    else if(value < sq4)
    {
        data[0] = (char)(255 * sin((value - sq3)/(sq4 - sq3) * M_PI * 0.5f));
        data[1] = 255;
        data[2] = 0;
    }
    else if(value < sq5)
    {
        data[0] = 255;
        data[1] = (char)(255 * max(0.0f, (sq5 - value)/(sq5 - sq4)));
        data[2] = 0;
    }
    else
    {
        data[0] = 255;
        data[1] = 0;
        data[2] = (char)(255 * min(0.8f, (value - sq5)/(sq6 - sq5)));
    }
}

void write_tga(const vector<vector<unsigned>> & field, const char * file, const unsigned particles)
{
    char header[] = { 0, // id length 0
                0, // no palette
                2, // rgb
                0, 0, // palette start
                0, 0, // palette length
                16, // palette entry size
                0, 0, // x origin
                0, 0, // y origin
                (char)(field.size() % 256), (char)(field.size() / 256), // width
                (char)(field.size() % 256), (char)(field.size() / 256), // height
                24, // bpp
                0 // no attribute-bits, origin bottom left
            };
    char color[] = { 0, 0, 0 }; 
                                
    ofstream o(file, ofstream::binary);
    o.write(header, sizeof(header));
    for(unsigned n = 0; n < field.size(); ++n)
        for(unsigned m = 0; m < field.size(); ++m)
        {
            if(field[n][m] == 0)
                color[0] = color[1] = color[2] = 192;
            else gradient((float)field[n][m] / particles, color);
            o.write(color, sizeof(color));
        }
    o.close();
}

int main(int args, char ** argv)
{
    if(args != 2)
    {
        cerr << "supply file name" << endl;
        exit(EXIT_FAILURE);
    }

    const unsigned size = 800;
    const unsigned particles = size * size / 5;
    srand(time(0));

    vector<vector<unsigned> > field(size, vector<unsigned>(size));
    
    field[size / 2][size / 2] = 1;
    field[size / 2][size / 2 + 1] = 2;

    for(unsigned n = 3; n <= particles; ++n)
    {
        unsigned x = rand() % size, y = rand() % size;
        
        if(field[x][y])
        {
            --n;
            continue;
        }
        
        while(true)
        {
            switch(rand() % 4)
            {
                case 0:
                    ++x == size ? x = 0 : x;
                    break;
                case 1:
                    --x == -1 ? x = size - 1 : x;
                    break;
                case 2:
                    ++y == size ? y = 0 : y;
                    break;
                case 3:
                    --y == -1 ? y = size - 1 : y;
                    break;
            }
            
            if(field[x + 1 == size ? 0 : x + 1][y] || field[x - 1 == -1 ? size - 1 : x - 1][y] || field[x][y + 1 == size ? 0 : y + 1] || field[x][y - 1 == -1 ? size - 1 : y - 1])
                break;
        }
        
        field[x][y] = n;
        
        if(n % (particles / 100) == 0)
        {
            cerr << "writing current cluster... (" << n * 100 / particles << "/100)" << endl;
            write_tga(field, (format("%s%03d.tga") % argv[1] % (n * 100 / particles)).str().c_str(), particles);
        }
    }
}
