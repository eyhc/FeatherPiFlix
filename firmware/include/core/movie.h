#ifndef FILM_H
#define FILM_H

#include <string>
#include <filesystem>

#include "cover.h"

namespace core
{
    /**
     * \brief Represents all metadata and assets associated with a movie.
     *
     * A Movie contains both descriptive metadata (title, director, cast, etc.)
     * and resource paths (poster images, video file).
     */
    class Movie {
    public:
        /**
         * \brief Construct a Film with full metadata.
         *
         * \param title Movie title
         * \param year Release year
         * \param producer Producer name
         * \param category Genre or category of the movie
         * \param cover Associated cover (normal/square)
         * \param director Director name
         * \param actors List of actors (string format)
         * \param synopsis Short description of the movie
         * \param duration Duration in minutes
         * \param video_file Filesystem path to the video file
         */
        Movie(std::string title, int year, std::string producer,
            std::string category, Cover cover, std::string director,
            std::string actors, std::string synopsis, int duration, 
            std::filesystem::path video_file);


        // --- Accessors ---
        std::string title() const;
        int year() const;
        std::string producer() const;
        std::string category() const;
        Cover cover() const;
        std::string director() const;
        std::string actors() const;
        std::string synopsis() const;
        int duration() const;

        /**
         * \brief Get duration formatted as "Hh Mmin".
         * \return Human-readable duration string.
         */
        std::string duration_str() const;

        std::filesystem::path video_file() const;

        // --- Mutators ---
        void set_title(const std::string title);
        void set_year(const int year);
        void set_producer(const std::string producer);
        void set_category(const std::string category);
        void set_cover(const Cover cover);
        void set_director(const std::string director);
        void set_synopsis(const std::string synopsis);
        void set_duration(const int duration);
        void set_actors(const std::string actors);
        void set_video_file(const std::filesystem::path path);

        // --- Display & Comparison ---
        /**
         * \brief Compare two movies by title only.
         * \param other Another film
         * \return true if they represent the same movie
         */
        bool equals(const Movie &other) const;

        /**
         * \brief Stringify minimal film info: title, release year, director,
         * actors, and if not empty, category and productor.
         * \return A "compact" string representation.
         */
        std::string to_string() const;

        /**
         * \brief Print all film metadata to stdout.
         */
        void print_full() const;

    protected:
        std::string _title;
        int _year;                       ///< Release year
        std::string _producer;
        std::string _category;           ///< Movie category/genre
        Cover _cover;                    ///< Associated cover
        std::string _director;
        std::string _actors;
        std::string _synopsis;
        int _duration;                   ///< Duration in minutes
        std::filesystem::path _video_file;
    };
} // namespace fc

#endif // FILM_H
