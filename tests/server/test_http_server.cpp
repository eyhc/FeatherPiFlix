#include "server/http_server.h"

#include "../utils.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <gd.h>

using namespace server;
using namespace std;

int main() {
    HttpServer server(8080, "../assets", "../assets/www");

    if (!server.start()) {
        cerr << "Echec du démarrage du server" << endl;
        return 1;
    }

    auto data = test::get_data("http://localhost:8080/test");
    std::string res(reinterpret_cast<const char*>(data.data()), data.size());
    assert(res == TEST_PLAIN);

    data = test::get_data("http://localhost:8080/check");
    res = string(reinterpret_cast<const char*>(data.data()), data.size());
    assert(res == TEST_HTML);

    data = test::get_data("http://localhost:8080/imgs/default_normal.jpg");
    ofstream fout("./src_temp.jpg", std::ios::binary);
    fout.write(reinterpret_cast<const char*>(data.data()), data.size());
    fout.close();
    gdImagePtr out = gdImageCreateFromFile("./src_temp.jpg");
    assert(gdImageSX(out) == 240);
    assert(gdImageSY(out) == 320);
    gdImageDestroy(out);
    remove("./src_temp.jpg");

    data = test::get_data("http://localhost:8080/assets/spectre.min.css");
    res = string(reinterpret_cast<const char*>(data.data()), data.size());
    assert(res.find("Spectre.css v0.5.9") != string::npos);

    server.stop();

    cout << "TEST HTTP SERVER: OK" << endl;
    return 0;
}
