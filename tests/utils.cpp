#include "utils.h"
#include <curl/curl.h>

std::vector<std::byte> test::get_data(std::string url) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURLcode res_code = CURLE_FAILED_INIT;
    CURL *curl = curl_easy_init();
    std::vector<std::byte> result;

    if (!curl) return result;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        static_cast<curl_write_callback>([](
            char *contents, size_t size, size_t nmemb, void *userdata
        ) -> size_t {
            auto *data = static_cast<std::vector<std::byte>*>(userdata);
            for (size_t i = 0; i < size * nmemb; i++)
                data->push_back((std::byte)contents[i]);
            
            return size * nmemb;
        }));
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res_code = curl_easy_perform(curl);
    if (res_code != CURLE_OK) {
        curl_global_cleanup();
        result.clear();
        return result;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return result;
}
