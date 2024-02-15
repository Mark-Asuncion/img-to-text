#include "Magick++.h"
#include "Magick++/Pixels.h"
#include "MagickCore/magick-type.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>

#define ASSIGN_OPT(opt) \
    i++; \
    if (argv[i] != NULL)\
        opt = argv[i];

#define CHECK_INVALID(err) \
    if (argv[i+1][0] == '-') {\
        wcout << err << '\n';\
        help();\
        std::exit(1);\
    }

#define IMG_CHAR L"░▒▓█"

using std::string, std::wcout, std::endl, std::wstring;
using namespace Magick;

void help() {
    wcout << "Usage:\n";
    wcout << "imgtotext [options] [file]\n";
    wcout << "help:\n";
    wcout << "-h, --help\tprints this message\n";
    wcout << "options:\n";
    wcout << "-s, --size\tresize the image before converting to text\n";
    wcout << "-c, --contrast\tapplies amount of contrast\n";
}

int main(int argc,char **argv) {
    // TODO
    // copy to clipboard
    std::setlocale(LC_ALL, "");

    InitializeMagick(*argv);
    string path;
    string size = "";
    double contrast = 50.0;

    for (int i=1; i<argc; i++) {
        string opt = argv[i];
        if (opt == "--input" || opt == "-i") {
            CHECK_INVALID("Invalid path name");
            ASSIGN_OPT(path);
        }
        else if (opt == "-h" || opt == "--help") {
            help();
            std::exit(0);
        }
        else if (opt == "--size" || opt == "-s") {
            CHECK_INVALID("Invalid Size argument");
            ASSIGN_OPT(size);
        }
        else if (opt == "--contrast" || opt == "-c") {
            CHECK_INVALID("Invalid contrast argument");
            string constr;
            ASSIGN_OPT(constr);
            char* s = &constr.back();
            contrast = strtod(&constr[0], &s);
        }
        else {
            if (argv[i][0] == '-') {
                wcout << "Invalid path name\n";
                help();
                std::exit(1);
            }
            path = argv[i];
        }
    }
    if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
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
        if (!size.empty())
            image.resize(Geometry(size));
        Geometry geo = image.size();
        size_t w = geo.width();
        size_t h = geo.height();

        image.sigmoidalContrast(1, contrast);
        // image.modifyImage();
        Pixels view(image);

        Quantum *pixel_cache = view.get(0,0,w,h);
        size_t i = 0;
        size_t buf_size = w*h+h;
        wchar_t* buf = (wchar_t*) malloc(( buf_size+1 ) * sizeof(wchar_t));
        buf[buf_size] = 0;

        wstring equ_char(IMG_CHAR);
        auto shade = [&equ_char](Quantum* p) {
            Quantum shd = *p / QuantumRange * 100.0;
            size_t ch_id = floor((double) shd
                         / ( 100.0 / (double)equ_char.length() ));
            size_t id = (ch_id == 0)? ch_id:ch_id-1;
            if (id < 0) id = 0;
            else if (id >= 3) id = 2;
            return equ_char[id];
        };

        wchar_t* pbuf = buf;

        while ( i<w*h ) {
            *pbuf = shade(pixel_cache);
            i++;
            pixel_cache+=3;
            pbuf++;
            if (i%w == 0) {
                *pbuf = '\n';
                pbuf++;
            }
        }
        // view.sync();
        // image.write( "image.png" );

        wcout << buf << endl;
        // TODO copy to clipboard
    }
    catch( Exception &error_ ) {
        wcout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}
