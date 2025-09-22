#include <csv.h>

#include "core/utils.h"

using namespace std;
using namespace core;


/**************************** PARSING *****************************/

// Parse stream row by row.
// If the callback throws FoundException, parsing stops immediately (early exit)
// Any parsing error throws a std::runtime_error
void core::csv::read(std::istream &in, row_callback on_row) {
    csv_parser p;

    // init csv parser
    if (csv_init(&p, 0) != 0) throw std::runtime_error("csv_init failed");
    
    // guard to ensure freeing memory
    struct CsvGuard {
        csv_parser* p;
        ~CsvGuard() { if (p) csv_free(p); }
    };
    CsvGuard guard{&p}; // use a guard for default freeing parser

    // create context for context
    struct ParseCtx {
        vector<string> current_row;
        csv::row_callback *cb;
    };
    ParseCtx ctx{{}, &on_row};
    char buf[1024];

    // libcsv callbacks

    // called by libcsv at the end of a field
    auto field_cb = [](void *s, size_t len, void *data) {
        auto ctx = static_cast<ParseCtx*>(data);
        ctx->current_row.emplace_back(static_cast<const char*>(s), len);
    };

    // called by libcsv at the end of a row
    auto row_cb = [](int c, void *data) {
        (void) c;
        auto ctx = static_cast<ParseCtx*>(data);
        if (ctx->cb) (*ctx->cb)(ctx->current_row);
        ctx->current_row.clear();
    };

    // try to parse chunk by chunk using libcsv and previous callbacks
    // stop parsing if catch FoundException
    try {
        // read all file chunk by chunk
        while (in.good()) {
            in.read(buf, sizeof(buf));
            std::streamsize n = in.gcount();
            if (n <= 0) break; // break if there is no readable character

            size_t m = static_cast<size_t>(n);
            if (csv_parse(&p, buf, m, field_cb, row_cb, &ctx) != m)
                throw std::runtime_error(csv_strerror(csv_error(&p)));
        }

        // treat leftover data
        if (csv_fini(&p, field_cb, row_cb, &ctx) != 0)
            throw std::runtime_error(csv_strerror(csv_error(&p)));
    }
    catch(const FoundException&) { /* do nothing, just stop parsing */ }
}


/**************************** WRITING *****************************/

void csv::write_field(ostream &out, const string &field) {
    // get needed size for ouput text field
    size_t needed = csv_write(nullptr, 0, field.c_str(), field.size());

    // normalize text as correct csv field
    std::vector<char> buff(needed);
    csv_write(buff.data(), needed, field.c_str(), field.size());

    // write it on the stream
    out.write(buff.data(), needed);
}

void csv::write_row(ostream &out, const vector<string> &fields) {
    for (size_t i = 0; i < fields.size(); i++) {
        write_field(out, fields[i]);
        if (i < fields.size() - 1) out << ",";
    }
    out << endl;
}

void csv::write(ostream &out, const vector<vector<string>> &data) {
    for (const auto &row: data)
        write_row(out,row);
}


/**************************** FIELD HANDLER *****************************/

optional<string> csv::get_field(
    istream &in, const string &id, 
    const string &id_col_name, const string &value_col_name
) {
    bool is_first = true;
    int id_col_index = -1;
    int value_col_index = -1;
    optional<string> result = nullopt;

    csv::row_callback rc = [&](vector<string> row) {
        // First get id and value column index
        if (is_first) {
            auto itt = find(row.begin(), row.end(), id_col_name);
            auto its = find(row.begin(), row.end(), value_col_name);
            if (itt != row.end() && its != row.end()) {
                id_col_index = distance(row.begin(), itt);
                value_col_index = distance(row.begin(), its);
            }

            if (id_col_index < 0) 
                throw runtime_error("Missing id_column_name: "+id_col_name);
            if (value_col_index < 0) 
                throw runtime_error("Missing field_colunm_name: "+value_col_name);

            is_first = false;
        }

        // Next, search a corresponding title and get synopsis
        else if (row[id_col_index] == id) {
            result = row[value_col_index];
            throw csv::FoundException();
        }
    };

    try {
        csv::read(in, rc);
        return result;
    } catch(const exception& e) {
        return nullopt;
    }
}

void csv::edit_field(
    istream &in, ostream &out, const string &id, const string &value, 
    const string &id_col_name, const string &value_col_name)
{
    bool is_first = true;
    int id_col_index = -1;
    int value_col_index = -1;

    bool updated = false;

    auto callback = [&](vector<string> &row) {
        // First get id and value column index
        if (is_first) {
            auto itt = find(row.begin(), row.end(), id_col_name);
            auto its = find(row.begin(), row.end(), value_col_name);
            if (itt != row.end() && its != row.end()) {
                id_col_index = distance(row.begin(), itt);
                value_col_index = distance(row.begin(), its);
            }

            if (id_col_index < 0) 
                throw runtime_error("Missing id_column_name: "+id_col_name);
            if (value_col_index < 0) 
                throw runtime_error("Missing field_colunm_name: "+value_col_name);

            is_first = false;
        }

        // Next, search a corresponding title and get synopsis
        else if (row[id_col_index] == id && !updated) {
            row[value_col_index] = value;
            updated = true;
        }

        // write the row immediately to output
        write_row(out, row);
    };

    try { 
        read(in, callback);
    }
    catch(const std::exception& e) { /* Ignore exceptions */ }
}

//----------------------------------------------------------------------------

string core::slug(const string &src) {
    // List of special characters to replace
    string old = "àâäéèëêîïùûüöôŷÿç";
    // Corresponding replacement characters
    string nw  = "aaaeeeeiiuuuooyyc";

    // First pass: replace special characters with ASCII equivalents
    string res = "";
    for (size_t i = 0; i < src.size(); i++) {
        size_t j = 0;
        for (; j < old.size(); j+=2) {
            // Check if current and next char match a special character pair
            if (i != src.size()-1 && src[i] == old[j] && src[i+1] == old[j+1]) {
                res += nw[j/2]; // Replace with corresponding ASCII char
                i++;            // skip the next half-letter
                break;
            }
        }

        // If no replacement was made, keep the original character
        if (j == old.size()) res += src[i];
    }
    
    // Second pass: lowercase all char and replace non-alphanumeric with '_'
    for (size_t i = 0; i < res.size(); i++) {
        res[i] = tolower(res[i]);
        if ((res[i] < 'a' || 'z' < res[i]) && (res[i] < '0' || '9' < res[i]))
            res[i] = '_';
    }
    
    return res;
}
