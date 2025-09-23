#include "core/img_format.h"

#include <curl/curl.h>
#include <gd.h>
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;

int main(void) {
    curl_global_init(CURL_GLOBAL_ALL);
    
    const string url = 
        "https://i.pinimg.com/originals/16/4b/e5/"
        "164be50616ddf5d05775930ee4b4b618.jpg";

    CURLcode res_code = CURLE_FAILED_INIT;
    CURL *curl = curl_easy_init();
    string result;

    if (!curl) return 1;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        static_cast<curl_write_callback>([](
            char *contents, size_t size, size_t nmemb, void *userdata
        ) -> size_t {
            auto *data = static_cast<std::string*>(userdata);
            data->append(contents, size * nmemb);
            return size * nmemb;
        }));
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res_code = curl_easy_perform(curl);
    if (res_code != CURLE_OK) {
        curl_global_cleanup();
        return 2;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    ofstream fout("./src_temp.jpg");
    fout << result;
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
