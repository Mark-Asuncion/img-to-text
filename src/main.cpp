#include "Magick++.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <climits>

#define ASSIGN_OPT(opt) \
    i++; \
    if (argv[i] != NULL)\
        opt = argv[i];\

#define PRINT_BUF_NL(buf, w, h) \
        for (size_t i=0;i<w*h;i++) {\
            wcout << buf[i];\
            if ((i+1)%w==0)\
                wcout << '\n';\
        }\

#define IMG_CHAR L"░▒▓"

using namespace std;
using namespace Magick;

int main(int argc,char **argv) {
    // TODO
    // copy to clipboard
    std::setlocale(LC_ALL, "");

    InitializeMagick(*argv);
    string path;
    string size = "32x";
    double contrast = 50.0;

    for (int i=1; i<argc; i++) {
        string opt = argv[i];
        if (opt == "-input") {
            ASSIGN_OPT(path);
        }
        else if (opt == "-size") {
            ASSIGN_OPT(size);
        }
        else if (opt == "-contrast") {
            string constr;
            ASSIGN_OPT(constr);
            char* s = &constr.back();
            contrast = strtod(&constr[0], &s);
        }
        else
            path = argv[i];
    }
    if (!filesystem::exists(path) || filesystem::is_directory(path)) {
        wcout << "File does not exist or is a directory" << endl;
        return 1;
    }
    else if (path.empty()) {
        wcout << "No input file provided" << endl;
        return 1;
    }

    Image image;
    try {
        image.read( path );

        image.modifyImage();
        image.resize(Geometry(size));
        Geometry geo = image.size();
        size_t w = geo.width();
        size_t h = geo.height();

        // color filtering
        image.channel(ChannelType::GrayChannel);
        image.sigmoidalContrast(1, contrast);
        /* image.level(60.0, 109.0); */

        Quantum *pixel_cache = image.getPixels(0,0,w,h);
        size_t i = 0;
        size_t buf_size = w*h+h;
        wchar_t* buf = (wchar_t*) malloc(( buf_size+1 ) * sizeof(wchar_t));
        buf[buf_size] = 0;

        wchar_t equ_char[4] = IMG_CHAR;
        auto shade = [equ_char](Quantum* p) {
            Quantum shd = *p / (float)USHRT_MAX * 100.0;
            size_t ch_id = floor((double) shd / 33.0);
            size_t id = (ch_id == 0)? ch_id:ch_id-1;
            if (id < 0) id = 0;
            else if (id >= 3) id = 2;
            return equ_char[id];
        };

        wchar_t* pbuf = buf;
        while ( i<w*h ) {
            *pbuf = shade(pixel_cache);
            i++;
            pixel_cache++;
            pbuf++;
            if (i%w == 0) {
                *pbuf = '\n';
                pbuf++;
            }
        }
        // image.syncPixels();
        // image.write( "logo.png" );

        wcout << buf << endl;
        // TODO copy to clipboard
    }
    catch( Exception &error_ ) {
        wcout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}
