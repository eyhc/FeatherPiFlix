#include "core/cover.h"
#include "core/movie.h"

#include <iostream>
#include <cassert>

using namespace std;

void test_cover() {
    core::Cover img("normal.png", "square.png");
    assert(img.to_string() == "Cover : normal.png | square.png");

    img.set_normal_path("normal.jpg");
    img.set_square_path("square.jpg");

    assert(img.normal_path() == "normal.jpg");
    assert(img.square_path() == "square.jpg");
}

void test_movie_accessors() {
    core::Movie m(
        "flix", 2015, "toto", "drama", core::Cover("path1.png", "path2.png"),
        "titi", "", "empty movie", 135, "data/movie.mp4"
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
    core::Movie m(
        "Flix", 2015, "", "drama", core::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(m.to_string() == "Flix - toto - drama - 2015");

    m.set_producer("titi");
    m.set_director("");
    assert(m.to_string() == "Flix - titi - drama - 2015");
}

void test_movie_mutators() {
    core::Movie m(
        "flix", 2015, "toto", "drama", core::Cover("path1.png", "path2.png"),
        "titi", "", "empty movie", 135, "data/movie.mp4"
    );

    string long_summary = "abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_";

    m.set_title("featherPi");
    m.set_year(1850);
    m.set_producer("tata");
    m.set_category("comedy");
    m.set_cover(core::Cover());
    m.set_director("toto");
    m.set_actors("jack & john");
    m.set_synopsis(long_summary);
    m.set_duration(25);
    m.set_video_file("movie2.mp4");

    assert(m.title() == "featherPi");
    assert(m.year() == 1850);
    assert(m.producer() == "tata");
    assert(m.category() == "comedy");
    assert(m.cover().normal_path() == core::Cover().normal_path());
    assert(m.cover().square_path() == core::Cover().square_path());
    assert(m.director() == "toto");
    assert(m.actors() == "jack & john");
    assert(m.synopsis() == long_summary);
    assert(m.duration() == 25);
    assert(m.duration_str() == "25min");
    assert(m.video_file() == "movie2.mp4");
}

void test_movie_equality() {
    core::Movie m(
        "Flix", 2015, "", "drama", core::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(m.equals(m));

    core::Movie m2(
        "Flix", 2015, "", "drama", core::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(&m != &m2);
    assert(m.equals(m2));

    m2.set_title("featherPi");
    assert(!m2.equals(m));
}

int main(void)
{
    test_cover();
    test_movie_accessors();
    test_movie_to_string();
    test_movie_mutators();
    test_movie_equality();

    cout << "TEST DATA : OK"  << endl;
    return 0;
}
