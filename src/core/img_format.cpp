#include "core/img_format.h"

#include <gd.h>
#include <iostream>

using namespace core;
using namespace std;

data::Cover image_format::create_covers (
    const string &img_src, 
    const string &output_name,
    const filesystem::path &output_dir
) {
    // try to open source img
    gdImagePtr src = gdImageCreateFromFile(img_src.c_str());
    if (!src) return data::Cover();

    // create squared and standard cover
    gdImagePtr standard = resize_exact(src, 240, 320);
    gdImagePtr squared = resize_exact(src, 240, 240);

    // compute image paths
    filesystem::path standard_dest = output_dir / (output_name + ".jpg");
    filesystem::path square_dest = output_dir / (output_name + "_square.jpg");

    // open output files
    FILE *fout = fopen(standard_dest.c_str(), "w");
    if (!fout) {
        gdImageDestroy(src);
        return data::Cover();
    }
    FILE *fout2 = fopen(square_dest.c_str(), "w");
    if (!fout2) {
        gdImageDestroy(src);
        fclose(fout);
        remove(standard_dest.c_str());
        return data::Cover();
    }

    // write img to file
    gdImageJpeg(standard, fout, 80);
    gdImageJpeg(squared, fout2, 80);

    // close files
    fclose(fout);
    fclose(fout2);

    // free memory
    gdImageDestroy(src);
    gdImageDestroy(standard);
    gdImageDestroy(squared);

    return data::Cover(standard_dest, square_dest);
}

gdImagePtr image_format::resize_to_width(gdImagePtr src, int width) {
    int src_width = gdImageSX(src);
    int src_height = gdImageSY(src);
    int dest_height = (width * src_height) / src_width;

    return resize_exact(src, width, dest_height);
}

gdImagePtr image_format::scale_image(gdImagePtr src, double ratio) {
    if (ratio < 0) ratio = -ratio;

    int src_width = gdImageSX(src);
    int src_height = gdImageSY(src);

    return resize_exact(src, src_width * ratio, src_height * ratio);
}

gdImagePtr image_format::resize_exact(gdImagePtr src, int width, int height) {
    int src_width = gdImageSX(src);
    int src_height = gdImageSY(src);

    gdImagePtr dest = gdImageCreateTrueColor(width, height);

    gdImageCopyResampled(dest, src, 0, 0, 0, 0, 
        width, height, src_width, src_height);

    return dest;
}
