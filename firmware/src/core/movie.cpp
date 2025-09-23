#include <iostream>
#include <fstream>

#include "core/movie.h"
#include "core/utils.h"

using namespace std;
using namespace core::data;

//----------------------------------------------------
//                   MOVIE
//----------------------------------------------------

Movie::Movie(
    const string &title, int year, const string &category, 
    const string &producer, const string &director, const string &actors,
    int duration, unique_ptr<SynopsisProvider> s_provider,
    const Cover &cover, const filesystem::path &video_file
): 
    _title(title), _year(year), _category(category), _producer(producer),
    _director(director), _actors(actors), _duration(duration),
    _synopsis(move(s_provider)), _cover(cover), _video_file(video_file) {}

Movie::Movie(
    const string &title, int year, const string &category, 
    const string &producer, const string &director,
    const string &actors, int duration, const string &synopsis, 
    const Cover &cover, const filesystem::path &video_file
): 
    _title(title), _year(year), _category(category), _producer(producer),
    _director(director), _actors(actors), _duration(duration), _cover(cover),
    _video_file(video_file)
{
    _synopsis = make_unique<DirectSynopsisProvider>(synopsis);
}

Movie::Movie(const Movie &m): 
    _title(m._title), _year(m._year), _category(m._category), 
    _producer(m._producer), _director(m._director), _actors(m._actors), 
    _duration(m._duration), _cover(m._cover), _video_file(m._video_file)
{
    _synopsis = make_unique<DirectSynopsisProvider>(m.synopsis());
}

string Movie::title() const    { return _title; }
int Movie::year() const        { return _year; }
string Movie::producer() const { return _producer; }
string Movie::category() const { return _category; }
Cover Movie::cover() const     { return _cover; }
string Movie::director() const { return _director; }
string Movie::actors() const   { return _actors; }
string Movie::synopsis() const { return _synopsis.get()->get_synopsis(); }
filesystem::path Movie::video_file() const { return _video_file; }
int Movie::duration() const    { return _duration; }
string Movie::duration_str() const {
    string res;
    if (_duration / 60 != 0)
        res = std::to_string(_duration / 60) + "h ";
    res += std::to_string(_duration % 60) + "min";
    return res;
}

void Movie::set_year(int year)                   { _year = year; }
void Movie::set_producer(const string &producer) { _producer = producer; }
void Movie::set_category(const string &category) { _category = category; }
void Movie::set_cover(const Cover &cover)        { _cover = cover; }
void Movie::set_director(const string &director) { _director = director; }
void Movie::set_duration(int duration)           { _duration = duration; }
void Movie::set_actors(const string &actors)     { _actors = actors; }
void Movie::set_synopsis(const string &synopsis) { 
    _synopsis.get()->set_synopsis(synopsis);
}
void Movie::set_video_file(const filesystem::path &path) { _video_file = path; }

void Movie::change_synopsis_provider(unique_ptr<SynopsisProvider> p) {
    _synopsis = move(p);
}

bool Movie::equals(const Movie &m) const {
    return _title.compare(m._title) == 0;
}

string Movie::to_string() const {
    string res = _title;
    if (! _director.empty())
        res += " - " + _director;
    if (! _producer.empty())
        res += " - " + _producer;
    if (! _category.empty())
        res += " - " + _category;
    if (! _actors.empty())
        res += " - " + _actors;
    res += " - " + std::to_string(_year);
    return res;
}

void Movie::print_full() const {
    cout << "title: " << _title << endl;
    cout << "date: " << _year << endl;
    cout << "duration: " << _duration << " (" << duration_str() << ")" << endl;
    cout << "category: " << _category << endl;
    cout << "director: " << _director << endl;
    cout << "producer: " << _producer << endl;
    cout << "actors: " << _actors << endl;
    cout << _cover.to_string() << endl;
    cout << "file: " << _video_file << endl;

    string short_summary = this->synopsis();
    if (short_summary.size() > 100) 
        short_summary.erase(100, short_summary.size()-100);
    cout << "synopsis: " << short_summary << "..." << endl;
}

//----------------------------------------------------
//                PROVIDERS
//----------------------------------------------------

DirectSynopsisProvider::DirectSynopsisProvider(const string &synopsis):
    _synopsis(synopsis) {}

string DirectSynopsisProvider::get_synopsis() const {
    return _synopsis;
}

void DirectSynopsisProvider::set_synopsis(const string &synopsis) {
    _synopsis = synopsis;
}

CSVFileSynopsisProvider::CSVFileSynopsisProvider(
    const string &movie_title, const string &csv_filename
): _title(movie_title), _csv_file(csv_filename) {}

string CSVFileSynopsisProvider::get_synopsis() const {
    ifstream fin(_csv_file);
    if (!fin.is_open()) throw runtime_error("Cannot open CSV file");

    auto res = csv::get_field(fin, _title, "title", "synopsis");
    fin.close();
    return (res.has_value()) ? res.value() : "";
}

void CSVFileSynopsisProvider::set_synopsis(const string &synopsis) {
    // 1. open csv file
    ifstream fin(_csv_file);
    if (!fin.is_open()) throw runtime_error("Cannot open CSV file");

    // 2. create ouptut file as temporary (before renamming)
    std::string temp_file = _csv_file + ".tmp";
    std::ofstream fout(temp_file);
    if (!fout.is_open())
        throw std::runtime_error("Cannot open temporary file for writing");

    // 3. edit synopsis
    csv::edit_field(fin, fout, _title, synopsis, "title", "synopsis");

    // 4. close files
    fin.close();
    fout.close();

    // 5. overwrite original file
    std::remove(_csv_file.c_str());
    std::rename(temp_file.c_str(), _csv_file.c_str());
}
