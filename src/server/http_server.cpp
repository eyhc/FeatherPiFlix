#include "server/http_server.h"

#include <fcntl.h>
#include <string>
#include <cstring>

using namespace server;
using namespace std;

HttpServer::HttpServer(
    int port,
    const filesystem::path &imgs_folder,
    const filesystem::path &assets_folder
): _port(port),
   _daemon(nullptr),
   _imgs_folder(imgs_folder),
   _assets_folder(assets_folder) {}

HttpServer::~HttpServer() {
    stop(); 
}

bool HttpServer::start()
{
    _daemon = MHD_start_daemon(
        MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD, _port, nullptr, nullptr,
        static_cast<MHD_AccessHandlerCallback>(callback), this, MHD_OPTION_END);

    if (!_daemon) 
        return false;
    return true;
}

bool HttpServer::is_started() {
    return _daemon != nullptr;
}

void HttpServer::stop() {
    if (_daemon) {
        MHD_stop_daemon(_daemon);
        _daemon = nullptr;
    }
}

MHD_Result HttpServer::get_callback(const char *url, struct MHD_Connection *conn) {
    MHD_Response *response = nullptr;
    int status = MHD_HTTP_OK;

    std::filesystem::path file_path;

    if (strcmp(url, "/") == 0) {
        response = make_text_response("<html><body><h1>Hello depuis /</h1></body></html>",
                                      "text/html; charset=utf-8");
    }
    else if (strncmp(url, "/imgs/", 6) == 0) {
        string s_url(url); s_url.erase(0, 6);
        file_path = _imgs_folder / s_url;
        response = make_file_response(file_path);
        if (!response) status = MHD_HTTP_NOT_FOUND;
    }
    else if (strncmp(url, "/assets/", 8) == 0) {
        string s_url(url); s_url.erase(0, 8);
        file_path = _assets_folder / s_url;
        response = make_file_response(file_path);
        if (!response) status = MHD_HTTP_NOT_FOUND;
    }
    else if (strcmp(url, "/test") == 0) {
        response = make_text_response(TEST_PLAIN, "text/plain; charset=utf-8");
    }
    else if (strcmp(url, "/check") == 0) {
        response = make_text_response(TEST_HTML, "text/html; charset=utf-8");
    }
    else {
        status = MHD_HTTP_NOT_FOUND;
    }

    if (status == MHD_HTTP_NOT_FOUND) {
        response = make_text_response(
            "404 - Not Found",
            "text/plain; charset=utf-8"
        );
    }

    if (!response) return MHD_NO;

    MHD_Result ret = MHD_queue_response(conn, status, response);
    MHD_destroy_response(response);
    return ret;
}

MHD_Result HttpServer::post_callback(
    const char *url,
    struct MHD_Connection *conn,
    const char *data,
    size_t data_size
) {
    (void)url; (void)conn; (void)data; (void)data_size;
    return MHD_NO;
}

MHD_Result HttpServer::callback(
    void *cls,
    struct MHD_Connection *conn,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    long unsigned int *upload_data_size,
    void **con_cls
) {
    (void)version; (void)con_cls;

    HttpServer *hs = static_cast<HttpServer *>(cls);

    if (strcmp(method, "GET") == 0)
        return hs->get_callback(url, conn);
    else if (strcmp(method, "POST") == 0)
        return hs->post_callback(url, conn, upload_data, *upload_data_size);
    else
        return MHD_NO;
}


MHD_Response* HttpServer::make_text_response(
    const std::string &content, const char *mime
) {
    MHD_Response *res = MHD_create_response_from_buffer(
        content.size(),
        (void *)content.c_str(),
        MHD_RESPMEM_MUST_COPY
    );
    MHD_add_response_header(res, "Content-Type", mime);
    return res;
}

MHD_Response* HttpServer::make_file_response(const std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) return nullptr;

    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) return nullptr;
    lseek(fd, 0, SEEK_SET);

    return MHD_create_response_from_fd(size, fd);
}
