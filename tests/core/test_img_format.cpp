#include "core/img_format.h"

#include <curl/curl.h>
#include <gd.h>
#include <fstream>
#include <iostream>
#include <cassert>

#include "../utils.h"

using namespace std;

int main(void) {
    const string url = 
        "https://i.pinimg.com/originals/16/4b/e5/"
        "164be50616ddf5d05775930ee4b4b618.jpg";

    const auto res = test::get_data(url);

    ofstream fout("./src_temp.jpg", std::ios::binary);
    fout.write(reinterpret_cast<const char*>(res.data()), res.size());
    fout.close();

    auto s = 
        core::image_format::create_covers("./src_temp.jpg", "img_sortie", ".");

    assert(s.normal_path() == "./img_sortie.jpg");
    assert(s.square_path() == "./img_sortie_square.jpg");

    gdImagePtr out = gdImageCreateFromFile(s.normal_path().c_str());
    assert(gdImageSX(out) == 240);
    assert(gdImageSY(out) == 320);
    gdImageDestroy(out);

    out = gdImageCreateFromFile(s.square_path().c_str());
    assert(gdImageSX(out) == 240);
    assert(gdImageSY(out) == 240);
    gdImageDestroy(out);
    
    remove("./src_temp.jpg");
    remove(s.normal_path().c_str());
    remove(s.square_path().c_str());

    cout << "TEST FORMATER IMG : OK" << endl;
    return 0;
}
