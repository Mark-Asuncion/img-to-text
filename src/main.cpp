#include "Magick++.h"
#include "Magick++/Color.h"
#include "Magick++/Functions.h"
#include "Magick++/Geometry.h"
#include "Magick++/Image.h"
#include "Magick++/Include.h"
#include "Magick++/Pixels.h"
#include "MagickCore/colorspace.h"
#include "MagickCore/image.h"
#include "MagickCore/pixel.h"
#include "MagickCore/magick-type.h"
#include "logger.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include "sample.c"

#define ASSIGN_OPT(opt, opt_name)                                   \
    i++;                                                            \
    if (argv[i] != NULL)                                            \
        opt = argv[i];                                              \
    else                                                            \
        std::cout << "[ERR]Missing Value of option " << opt_name;

#define HELP                                                                                                   \
    std::cout << "Usage:\n";                                                                                   \
    std::cout << "toascii [options] [file]\n";                                                                 \
    std::cout << "help:\n";                                                                                    \
    std::cout << "-h, --help\tprints this message\n";                                                          \
    std::cout << "options:\n";                                                                                 \
    std::cout << "-o, --output <path>   Save Output Image to <path>. Default is current directory\n";  \
    // std::cout << "[Unused]-c, --contrast        applies amount of contrast\n";

#define CHECK_INVALID(err)                     \
    if (argv[i+1][0] == '-') {                 \
        std::cout << "[ERR]" << err << '\n';   \
        HELP;                                  \
        std::exit(1);                          \
    }

using namespace Magick;

int main(int argc,char **argv) {
    Magick::InitializeMagick(*argv);
    std::string path;
    std::string size = "";
    std::string output_name = "";
    // double contrast = 50.0;

    for (int i=1; i<argc; i++) {
        std::string opt = argv[i];
        if (opt == "-h" || opt == "--help") {
            HELP;
            std::exit(0);
        }
        // else if (opt == "--contrast" || opt == "-c") {
        //     CHECK_INVALID("Invalid contrast argument");
        //     std::string constr;
        //     ASSIGN_OPT(constr, "--contrast, -c");
        //     char* s = &constr.back();
        //     contrast = strtod(&constr[0], &s);
        // }
        else if (opt == "--output" || opt == "-o") {
            std::string out;
            ASSIGN_OPT(out, "--output, -o");
            output_name = out;
        }
        else {
            if (argv[i][0] == '-') {
                std::cout << "[ERR]Argument is not a valid option\n";
                HELP;
                std::exit(1);
            }
            path = argv[i];
        }
    }
    if (path.empty()) {
        std::cout << "[ERR]No input file provided" << std::endl;
        return 1;
    }
    else if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
        std::cout << "[ERR]File does not exist or is a directory" << std::endl;
        return 1;
    }

    {
        bool is_dir = (output_name.empty())? false:std::filesystem::is_directory(output_name);
        if (output_name.empty() || is_dir) {
            std::string dir = (is_dir)? output_name:"";
            {
                size_t pos = dir.find_last_of('/');
                if (pos != std::string::npos) {
                    dir = dir.substr(0, pos);
                }
            }
            size_t pos = path.find_last_of('/');
            std::string name;
            if (pos != std::string::npos) {
                name = path.substr(pos+1);
            }
            else {
                name = path;
            }
            pos = name.find(".");
            if (pos == std::string::npos) {
                std::cout << "[ERR]Invalid Filename\n";
                exit(1);
            }
            output_name = name.substr(0, pos) + "-ascii" + name.substr(pos);
            if (is_dir) {
                output_name = dir + "/" + output_name;
            }
        }
    }

    try {
        Magick::Image sample, input;
        {
            {
                sample.read(
                    sample_raw.width,
                    sample_raw.height,
                    "RGB",
                    Magick::StorageType::CharPixel,
                    sample_raw.pixel_data
                );
                sample.grayscale(MagickCore::PixelIntensityMethod::UndefinedPixelIntensityMethod);
            }

            {
                input.read(path);
                Magick::Image output(
                    Magick::Geometry( input.columns(), input.rows() ),
                    Magick::Color("black")
                );
                output.grayscale(MagickCore::PixelIntensityMethod::UndefinedPixelIntensityMethod);

                size_t tile_size = sample_raw.height;
                {
                    Magick::Geometry size = Magick::Geometry(input.columns(),input.rows());
                    input.resize(Magick::Geometry(size.width()/tile_size, size.height()/tile_size));
                    size = Magick::Geometry(input.columns(),input.rows());
                    input.resize(Magick::Geometry(size.width()*tile_size, size.height()*tile_size));

                    input.grayscale(MagickCore::PixelIntensityMethod::Rec709LumaPixelIntensityMethod);
                }

                Magick::Geometry input_tile_size = Magick::Geometry(input.columns()/tile_size,input.rows()/tile_size);

                output.modifyImage();
                {
                    Magick::Pixels input_view(input);
                    Magick::Pixels sample_view(sample);
                    Magick::Pixels output_view(output);

                    for (size_t row = 0; row < input_tile_size.width(); row++) {
                        for (size_t col = 0; col < input_tile_size.height(); col++) {
                            size_t center_index = tile_size * (tile_size/2) + ( tile_size / 2 );
                            MagickCore::Quantum* pixels = input_view.get(row*tile_size, col*tile_size, tile_size, tile_size);
                            size_t sample_index =  (( MagickCore::Quantum ) (pixels[center_index] / QuantumRange)) * 9;

                            MagickCore::Quantum* output_pixels = output_view.get(row*tile_size,col*tile_size, tile_size, tile_size);
                            MagickCore::Quantum* sample_pixels = sample_view.get(tile_size*sample_index, 0, tile_size, tile_size);

                            for (size_t p = 0;p<tile_size*tile_size;p++) {
                                *output_pixels++ = *sample_pixels++;
                            }

                            output_view.sync();
                        }
                    }
                }
                output.colorSpace(ColorspaceType::sRGBColorspace);
                output.type(ImageType::TrueColorType);
                INFO_LOG("%s", output_name.c_str());
                std::cout << "Output Saving to " << output_name << '\n';
                output.write(output_name);
            }
        }
    }
    catch( Magick::Exception &error_ ) {
        std::cout << "Caught exception: " << error_.what() << std::endl;
        Magick::TerminateMagick();
        return 1;
    }
    Magick::TerminateMagick();

    return 0;
}
