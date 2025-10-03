#include "server/router.h"

using namespace std;
using namespace server;

#define T_404 "404 - NOT FOUND"

void routing::Router::add_route(
    const std::string &method,
    const std::regex &regex_path,
    Handler h
) {
    _routes.push_back({ regex_path, method, h });
}

optional<routing::Route> routing::Router::get_route(
    const string &method, const string &url
) {
    for (auto &r: _routes) {
        if (r.method == method && regex_match(url, r.pattern))
            return r;
    }
    return nullopt;
}

routing::Response routing::Router::dispatch(const routing::Request &req) {
    auto route = get_route(req.method, req.url);

    if (route.has_value())
        return route.value().handler(req);

    MHD_Response *r = MHD_create_response_from_buffer(
                          sizeof(T_404), (void *)T_404, MHD_RESPMEM_MUST_COPY);

    return {r, MHD_HTTP_NOT_FOUND};
}
