#include <fstream>
#include <csv.h>
#include <cstring>

#include "core/catalog.h"
#include "core/csv.h"

using namespace std;
using namespace core;

Catalog::Catalog() = default;

Catalog::Catalog(const string& filename) {
    ifstream in(filename);

    bool is_first = true;
    int title_index = -1;
    int year_index = -1;
    int category_index = -1;
    int cover_normal_index = -1;
    int cover_square_index = -1;
    int director_index = -1;
    int producer_index = -1;
    int actors_index = -1;
    int synopsis_index = -1;
    int video_file_index = -1;
    int duration_index = -1;
    
    csv::row_callback on_row = [&](vector<string> row) {
        // First get id and value column index
        if (is_first) {
            auto itt  = find(row.begin(), row.end(), "title");
            auto ity  = find(row.begin(), row.end(), "year");
            auto itc  = find(row.begin(), row.end(), "category");
            auto itd  = find(row.begin(), row.end(), "director");
            auto itp  = find(row.begin(), row.end(), "producer");
            auto itdt = find(row.begin(), row.end(), "duration");
            auto ita  = find(row.begin(), row.end(), "actors");
            auto its  = find(row.begin(), row.end(), "synopsis");
            auto itf  = find(row.begin(), row.end(), "video_file");
            auto itnc = find(row.begin(), row.end(), "normal_cover");
            auto itsc = find(row.begin(), row.end(), "squared_cover");

            if (itt != row.end()
                && ity  != row.end()
                && itc  != row.end()
                && itd  != row.end()
                && itp  != row.end()
                && itdt != row.end()
                && ita  != row.end()
                && its  != row.end()
                && itf  != row.end()
                && itnc  != row.end()
                && itsc  != row.end()
            ) {
                title_index        = distance(row.begin(), itt);
                year_index         = distance(row.begin(), ity);
                category_index     = distance(row.begin(), itc);
                director_index     = distance(row.begin(), itd);
                producer_index     = distance(row.begin(), itp);
                duration_index     = distance(row.begin(), itdt);
                actors_index       = distance(row.begin(), ita);
                synopsis_index     = distance(row.begin(), its);
                video_file_index   = distance(row.begin(), itf);
                cover_normal_index = distance(row.begin(), itnc);
                cover_square_index = distance(row.begin(), itsc);
            }
            else throw runtime_error("Missing column(s)");

            is_first = false;
        }

        // Next, convert data to movie
        else  {
            auto m = make_unique<data::FullMovie>(
                row[title_index],
                atoi(row[year_index].c_str()),
                row[producer_index],
                row[category_index], 
                data::Cover(row[cover_normal_index], row[cover_square_index]),
                row[director_index],
                row[actors_index],
                row[synopsis_index],
                atoi(row[duration_index].c_str()),
                row[video_file_index]
            );

            add(move(m));
        }
    };

    try {
        csv::read(in, on_row);
    }
    catch(const std::exception& e) { /* Ignore exception */ }
    

    in.close();
}

void Catalog::add(unique_ptr<data::Movie> m) {
    // checks first if already exist in this catalog
    if (!get_movie(m.get()->title()))
        _data.push_back(std::move(m));
}

void Catalog::remove(const string &title) {
    size_t i = 0;
    while (i < _data.size() && _data[i].get()->title() != title) i++;
    
    if (i != _data.size())
        _data.erase(next(_data.begin(), i));
}

size_t Catalog::size() const { return _data.size(); }

data::Movie *Catalog::get_movie(const std::string& title) {
    for (const auto &m: _data) {
        if (m.get()->title() == title) {
            return m.get();
        }
    }
    return nullptr;
}

vector<data::Movie *> Catalog::all_movies() {
    vector<data::Movie*> v;
    for (const auto &movie: _data) {
        v.push_back(movie.get());
    }
    return v;
}

void Catalog::save(const string &filename) const {
    ofstream out(filename);

    // write headers
    csv::write_row(out, {
        "title",
        "year",
        "category",
        "director",
        "producer",
        "duration",
        "actors",
        "synopsis",
        "video_file",
        "normal_cover",
        "squared_cover"
    });

    // write all data
    for (auto &mv: _data) {
        data::Movie *movie = mv.get();
        string synopsis = movie->synopsis().has_value() ?
                            movie->synopsis().value() : "";
        csv::write_row(out, {
            movie->title(),
            to_string(movie->year()),
            movie->category(),
            movie->director(),
            movie->producer(),
            to_string(movie->duration()),
            movie->actors(),
            synopsis,
            movie->video_file().generic_string(),
            movie->cover().normal_path().generic_string(),
            movie->cover().square_path().generic_string()
        });
    }

    out.close();
}
