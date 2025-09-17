#include "core/movie.h"

#include <iostream>

using namespace std;
using namespace core;

Movie::Movie(std::string title, int year, std::string producer,
    std::string category, Cover cover, std::string director,
    std::string actors, std::string synopsis, int duration, 
    std::filesystem::path video_file)
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

string Movie::title() const    { return _title; }
int Movie::year() const        { return _year; }
string Movie::producer() const { return _producer; }
string Movie::category() const { return _category; }
Cover Movie::cover() const     { return _cover; }
string Movie::director() const { return _director; }
string Movie::actors() const   { return _actors; }
string Movie::synopsis() const { return _synopsis; }
filesystem::path Movie::video_file() const { return _video_file; }
int Movie::duration() const    { return _duration; }
string Movie::duration_str() const {
    string res;
    if (_duration / 60 != 0)
        res = std::to_string(_duration / 60) + "h ";
    res += std::to_string(_duration % 60) + "min";
    return res;
}

void Movie::set_title(string title)       { _title = title; }
void Movie::set_year(int year)            { _year = year; }
void Movie::set_producer(string producer) { _producer = producer; }
void Movie::set_category(string category) { _category = category; }
void Movie::set_cover(Cover cover)        { _cover = cover; }
void Movie::set_director(string director) { _director = director; }
void Movie::set_synopsis(string synopsis) { _synopsis = synopsis; }
void Movie::set_duration(int duration)    { _duration = duration; }
void Movie::set_actors(string actors)     { _actors = actors; }
void Movie::set_video_file(filesystem::path path) { _video_file = path;}

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
    cout << "titre: " << _title << endl;
    cout << "date: " << _year << endl;
    cout << "duration: " << _duration << " (" << duration_str() << ")" << endl;
    cout << "category: " << _category << endl;
    cout << "director: " << _director << endl;
    cout << "producer: " << _producer << endl;
    cout << "actors: " << _actors << endl;
    cout << _cover.to_string() << endl;
    cout << "file: " << _video_file << endl;

    string short_summary = _synopsis;
    if (short_summary.size() > 100) 
        short_summary.erase(100, short_summary.size()-100);
    cout << "synopsis: " << short_summary << "..." << endl;
}
