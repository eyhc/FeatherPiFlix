#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <microhttpd.h>
#include <filesystem>

#include "core/media_manager.h"

#define TEST_HTML "<html><body><h1>FeatherPiFlix</h1><p>My new software!</p>\
                   </body></html>"

#define TEST_PLAIN "Awesome FeatherPiFlix !"

namespace server {
    class HttpServer {
    public:
        HttpServer(
            int port,
            const std::filesystem::path &imgs_folder,
            const std::filesystem::path &assets_folder
        );
        ~HttpServer();
        bool start();
        bool is_started();
        void stop();

    protected:
        MHD_Result get_callback(const char *url, struct MHD_Connection *conn);
        MHD_Result post_callback(
            const char *url,
            struct MHD_Connection *conn,
            const char *data,
            size_t data_size
        );

        static MHD_Result callback(
            void *cls,
            struct MHD_Connection *conn,
            const char *url,
            const char *method,
            const char *version,
            const char *upload_data,
            long unsigned int *upload_data_size,
            void **con_cls
        );

        // don't forger to free memory
        static MHD_Response* make_text_response(
            const std::string &body, const char *mime);
        static MHD_Response* make_file_response(const std::string &path);

    private:
        const int _port;
        struct MHD_Daemon *_daemon;
        const std::filesystem::path _imgs_folder;
        const std::filesystem::path _assets_folder;
    };

} // namespace server

#endif // HTTP_SERVER_H
