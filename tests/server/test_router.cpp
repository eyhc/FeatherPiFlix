#include "server/router.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace server;


routing::Response handler(const routing::Request &r) {
    (void) r;
    return { NULL, 400 };
}

routing::Response handler2(const routing::Request &r) {
    (void) r;
    return { NULL, 500 };
}

int main(void) {

    routing::Router router;

    router.add_route("GET", regex("^/imgs/.*$"), handler);
    router.add_route("POST", regex("^/api/movie$"), handler2);

    auto r = router.get_route("GET", "/imgs/tt.png");
    assert(r.has_value());

    routing::Response res = r.value().handler({ "GET", "toto", NULL });
    assert(res.status == 400);

    r = router.get_route("GET", "/imgs/repo/toto.jpg");
    assert(r.has_value());

    res = r.value().handler({ "GET", "toto", NULL });
    assert(res.status == 400);

    r = router.get_route("POST", "/api/movie");
    assert(r.has_value());

    res = r.value().handler({ "GET", "toto", NULL });
    assert(res.status == 500);

    std::cout << "TEST ROUTER: OK" << endl;
}
