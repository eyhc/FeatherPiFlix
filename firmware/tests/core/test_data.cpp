#include "core/cover.h"
#include "core/movie.h"
#include "core/utils.h"

#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;
using namespace core;

void test_cover() {
    data::Cover img("normal.png", "square.png");
    assert(img.to_string() == "Cover : normal.png | square.png");

    img.set_normal_path("normal.jpg");
    img.set_square_path("square.jpg");

    assert(img.normal_path() == "normal.jpg");
    assert(img.square_path() == "square.jpg");
}

void test_movie_accessors() {
    data::Movie m(
        "flix", 2015, "drama", "toto", "titi", "", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );

    assert(m.title() == "flix");
    assert(m.year() == 2015);
    assert(m.producer() == "toto");
    assert(m.category() == "drama");
    assert(m.cover().normal_path() == "path1.png");
    assert(m.cover().square_path() == "path2.png");
    assert(m.director() == "titi");
    assert(m.actors().empty());
    assert(m.synopsis() == "empty movie");
    assert(m.duration() == 135);
    assert(m.duration_str() == "2h 15min");
    assert(m.video_file() == "data/movie.mp4");
}

void test_movie_to_string() {
    data::Movie m(
        "Flix", 2015, "drama", "toto", "titi", "", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );

    assert(m.to_string() == "Flix - titi - toto - drama - 2015");

    m.set_producer("tyty");
    m.set_director("");
    assert(m.to_string() == "Flix - tyty - drama - 2015");
}

void test_movie_mutators() {
    data::Movie m(
        "Flix", 2015, "drama", "toto", "titi", "tutu, tete", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );

    string long_summary = "abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_";

    m.set_year(1850);
    m.set_producer("tata");
    m.set_category("comedy");
    m.set_cover(data::Cover());
    m.set_director("toto");
    m.set_actors("jack & john");
    m.set_synopsis(long_summary);
    m.set_duration(25);
    m.set_video_file("movie2.mp4");

    assert(m.title() == "Flix");
    assert(m.year() == 1850);
    assert(m.producer() == "tata");
    assert(m.category() == "comedy");
    assert(m.cover().normal_path() == data::Cover().normal_path());
    assert(m.cover().square_path() == data::Cover().square_path());
    assert(m.director() == "toto");
    assert(m.actors() == "jack & john");
    assert(m.synopsis() == long_summary);
    assert(m.duration() == 25);
    assert(m.duration_str() == "25min");
    assert(m.video_file() == "movie2.mp4");
}

void test_movie_equality() {
    data::Movie m(
        "Flix", 2015, "drama", "toto", "titi", "tutu, tete", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );

    assert(m.equals(m));

    data::Movie m2(
        "Flix", 2015, "drama", "toto", "titi", "tutu, tete", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );

    assert(&m != &m2);
    assert(m.equals(m2));

    data::Movie m3(
        "PiFlix", 2015, "drama", "toto", "titi", "tutu, tete", 135, "empty movie",
        data::Cover("path1.png", "path2.png"), "data/movie.mp4"
    );
    assert(!m3.equals(m));
}

void test_lazy_synopsis() {
    const string synopsis1 = "Long time ago...";
    const string synopsis2 = "efg\nh";
    const string synopsis3 = "\"go\r\nnot'";

    // create correct csv files
    const string file = "synopsis.csv";
    ofstream out(file);
    csv::write_row(out, {"title", "synopsis", "other"});
    csv::write(out, {
        {"f1", synopsis1, "bad"}, 
        {"f2", synopsis2, "good..\n."},
        {"f3", synopsis3, "ok"}
    });
    out.close();

    const string file2 = "synopsis2.csv";
    out.open(file2);
    csv::write_row(out, {"synopsis", "other", "title"});
    csv::write(out, {
        {synopsis1, "bad", "f4"}, 
        {synopsis2, "good..\n.", "f5"},
    });
    out.close();

    /// create lazy movies with previous file
    auto s = make_unique<data::CSVFileSynopsisProvider>("f1", file);
    auto f1 = data::Movie(
        "f1", 2015, "", "", "", "", 10, move(s), data::Cover(), "");

    s = make_unique<data::CSVFileSynopsisProvider>("f2", file);
    auto f2 = data::Movie(
        "f2", 2015, "", "", "", "", 10, move(s), data::Cover(), "");

    s = make_unique<data::CSVFileSynopsisProvider>("f3", file);
    auto f3 = data::Movie(
        "f3", 2015, "", "", "", "", 10, move(s), data::Cover(), "");

    s = make_unique<data::CSVFileSynopsisProvider>("f4", file2);
    auto f4 = data::Movie(
        "f4", 2015, "", "", "", "", 10, move(s), data::Cover(), "");

    s = make_unique<data::CSVFileSynopsisProvider>("f5", file2);
    auto f5 = data::Movie(
        "f5", 2015, "", "", "", "", 10, move(s), data::Cover(), "");

    // check synopsis correctness
    assert(f1.synopsis() == synopsis1);
    assert(f2.synopsis() == synopsis2);
    assert(f3.synopsis() == synopsis3);
    assert(f4.synopsis() == synopsis1);
    assert(f5.synopsis() == synopsis2);

    f2.set_synopsis("toto titi \n tata");
    assert(f2.synopsis() == "toto titi \n tata");
    remove(file.c_str());
    remove(file2.c_str());
}

void test_change_provider() {
    unique_ptr<data::SynopsisProvider> s = 
        make_unique<data::DirectSynopsisProvider>("synopsis1");
    data::Movie m("f", 1925, "", "", "", "", 37, move(s), data::Cover(), "");
    assert(m.synopsis() == "synopsis1");

    s = make_unique<data::DirectSynopsisProvider>("synopsis2");
    auto f = [&s](unique_ptr<data::SynopsisProvider>) -> 
    unique_ptr<data::SynopsisProvider> {
        return move(s);
    };
    m.change_synopsis_provider(f);
    assert(m.synopsis() == "synopsis2");
}

int main(void) {
    test_cover();
    test_movie_accessors();
    test_movie_to_string();
    test_movie_mutators();
    test_movie_equality();
    test_lazy_synopsis();
    test_change_provider();

    cout << "TEST DATA : OK"  << endl;
    return 0;
}
