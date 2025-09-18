#include <iostream>
#include <fstream>

#include "core/movie.h"
#include "core/csv.h"

using namespace std;
using namespace core::data;

//----------------------------------------------------
//               BASE MOVIE CLASS
//----------------------------------------------------

string MovieBase::title() const    { return _title; }
int MovieBase::year() const        { return _year; }
string MovieBase::producer() const { return _producer; }
string MovieBase::category() const { return _category; }
Cover MovieBase::cover() const     { return _cover; }
string MovieBase::director() const { return _director; }
string MovieBase::actors() const   { return _actors; }
filesystem::path MovieBase::video_file() const { return _video_file; }
int MovieBase::duration() const    { return _duration; }
string MovieBase::duration_str() const {
    string res;
    if (_duration / 60 != 0)
        res = std::to_string(_duration / 60) + "h ";
    res += std::to_string(_duration % 60) + "min";
    return res;
}

void MovieBase::set_year(int year)            { _year = year; }
void MovieBase::set_producer(string producer) { _producer = producer; }
void MovieBase::set_category(string category) { _category = category; }
void MovieBase::set_cover(Cover cover)        { _cover = cover; }
void MovieBase::set_director(string director) { _director = director; }
void MovieBase::set_duration(int duration)    { _duration = duration; }
void MovieBase::set_actors(string actors)     { _actors = actors; }
void MovieBase::set_video_file(filesystem::path path) { _video_file = path;}

bool MovieBase::equals(const MovieBase &m) const {
    return _title.compare(m._title) == 0;
}

string MovieBase::to_string() const {
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

void MovieBase::print_full() const {
    cout << "titre: " << _title << endl;
    cout << "date: " << _year << endl;
    cout << "duration: " << _duration << " (" << duration_str() << ")" << endl;
    cout << "category: " << _category << endl;
    cout << "director: " << _director << endl;
    cout << "producer: " << _producer << endl;
    cout << "actors: " << _actors << endl;
    cout << _cover.to_string() << endl;
    cout << "file: " << _video_file << endl;

    auto summary = this->synopsis();
    if (summary.has_value()) {
        string short_summary = summary.value();
        if (short_summary.size() > 100) 
            short_summary.erase(100, short_summary.size()-100);
        cout << "synopsis: " << short_summary << "..." << endl;
    }
}

//----------------------------------------------------
//                FULL MOVIE CLASS
//----------------------------------------------------

FullMovie::FullMovie(string title, int year, string producer, string category, 
    Cover cover, string director, string actors, string synopsis, int duration, 
    filesystem::path video_file)
{
    _title = title;
    _year = year;
    _producer = producer;
    _category = category;
    _director = director;
    _actors = actors;
    _synopsis = synopsis;
    _duration = duration;
    _cover = cover;
    _video_file = video_file;
}

optional<string> FullMovie::synopsis() const { return _synopsis; }
void FullMovie::set_synopsis(string synopsis) { _synopsis = synopsis; }

//----------------------------------------------------
//                LAZY MOVIE CLASS
//----------------------------------------------------

LazyMovie::LazyMovie(string title, int year, string producer, string category,
    Cover cover, string director, string actors, filesystem::path csv_file, 
    int duration, filesystem::path video_file)
{
    _title = title;
    _year = year;
    _producer = producer;
    _category = category;
    _director = director;
    _actors = actors;
    _duration = duration;
    _cover = cover;
    _video_file = video_file;
    _csv_file = csv_file;
}

optional<string> LazyMovie::synopsis() const {
    ifstream fin(_csv_file);
    if (!fin.is_open()) throw runtime_error("Cannot open CSV file");

    auto res = csv::get_field(fin, _title, "title", "synopsis");
    fin.close();
    return res;
}

void LazyMovie::set_synopsis(string synopsis) {
    // open csv file
    ifstream fin(_csv_file);
    if (!fin.is_open()) throw runtime_error("Cannot open CSV file");

    // create ouptut file as temporary (before renamming)
    std::string temp_file = _csv_file + ".tmp";
    std::ofstream fout(temp_file);
    if (!fout.is_open())
        throw std::runtime_error("Cannot open temporary file for writing");

    // edit synopsis
    csv::edit_field(fin, fout, _title, synopsis, "title", "synopsis");

    // close files
    fin.close();
    fout.close();

    // overwrite original file
    std::remove(_csv_file.c_str());
    std::rename(temp_file.c_str(), _csv_file.c_str());
}
