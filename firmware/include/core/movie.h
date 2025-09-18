#ifndef FILM_H
#define FILM_H

#include <string>
#include <filesystem>
#include <optional>

#include "cover.h"

/**
 * \file movie.h
 * \brief Defines abstract and concrete movie classes with metadata management.
 */

namespace core::data {

//----------------------------------------------------
//               BASE MOVIE CLASS
//----------------------------------------------------

    /**
     * \class Movie
     * \brief Abstract base class for movie metadata.
     *
     * Stores and manages the basic information of a movie,
     * including title, year, producer, category, director, actors,
     * cover, duration, and associated video file.
     * Provides abstract methods for synopsis handling,
     * as well as formatting and comparison utilities.
     */
    class Movie {
    public:
        // --- Accessors ---

        /**
         * \brief Get the title of the movie.
         * \return Movie title.
         */
        std::string title() const;

        /**
         * \brief Get the release year of the movie.
         * \return Release year.
         */
        int year() const;

        /**
         * \brief Get the producer of the movie.
         * \return Producer name.
         */
        std::string producer() const;

        /**
         * \brief Get the category/genre of the movie.
         * \return Category name.
         */
        std::string category() const;

        /**
         * \brief Get the cover of the movie.
         * \return Cover object.
         */
        Cover cover() const;

        /**
         * \brief Get the director of the movie.
         * \return Director name.
         */
        std::string director() const;

        /**
         * \brief Get the actors of the movie.
         * \return Actors list in string format.
         */
        std::string actors() const;

        /**
         * \brief Get the synopsis of the movie.
         * \return Short description of the movie.
         */
        virtual std::optional<std::string> synopsis() const = 0;

        /**
         * \brief Get the duration of the movie in minutes.
         * \return Duration in minutes.
         */
        int duration() const;

        /**
         * \brief Get duration formatted as "Hh Mmin".
         * \return Human-readable duration string.
         */
        std::string duration_str() const;

        /**
         * \brief Get the filesystem path to the video file.
         * \return Video file path.
         */
        std::filesystem::path video_file() const;


        // --- Mutators ---

        /**
         * \brief Set the release year of the movie.
         * \param year New release year.
         *
         * \note No validation is performed, but a year between 1900 and 
         * the current year is recommended.
         */
        void set_year(const int year);

        /**
         * \brief Set the producer of the movie.
         * \param producer New producer name.
         */
        void set_producer(const std::string producer);

        /**
         * \brief Set the category/genre of the movie.
         * \param category New category.
         */
        void set_category(const std::string category);

        /**
         * \brief Set the cover of the movie.
         * \param cover New cover object.
         */
        void set_cover(const Cover cover);

        /**
         * \brief Set the director of the movie.
         * \param director New director name.
         */
        void set_director(const std::string director);

        /**
         * \brief Set the synopsis of the movie.
         * \param synopsis New short description.
         */
        virtual void set_synopsis(const std::string synopsis) = 0;

        /**
         * \brief Set the duration of the movie in minutes.
         * \param duration Duration in minutes.
         */
        void set_duration(const int duration);

        /**
         * \brief Set the actors of the movie.
         * \param actors New list of actors (string format).
         */
        void set_actors(const std::string actors);

        /**
         * \brief Set the video file path of the movie.
         * \param path New filesystem path to the video file.
         * 
         * \note No warranty is provided regarding the existence of the file.
         */
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

        /**
         * \brief Virtual default destructor needed by unique_ptr.
         */
        virtual ~Movie() = default; 

    protected:
        std::string _title;                ///< Movie title
        int _year;                         ///< Release year
        std::string _producer;             ///< Producer name
        std::string _category;             ///< Movie category/genre
        Cover _cover;                      ///< Associated cover
        std::string _director;             ///< Director name
        std::string _actors;               ///< Actors list in string format
        int _duration;                     ///< Duration in minutes
        std::filesystem::path _video_file; ///< Video file path
    };


//----------------------------------------------------
//               FULL MOVIE CLASS
//----------------------------------------------------

    /**
     * \class FullMovie
     * \brief Concrete implementation of Movie with full metadata.
     *
     * Stores a complete set of information about the movie,
     * including a detailed synopsis.
     */
    class FullMovie: public Movie {
    public:
        /**
         * \brief Construct a movie with full metadata.
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
        FullMovie(std::string title, int year, std::string producer,
            std::string category, Cover cover, std::string director,
            std::string actors, std::string synopsis, int duration, 
            std::filesystem::path video_file);

        /**
         * \brief Get the synopsis of the movie.
         * \return Short description of the movie.
         */
        std::optional<std::string> synopsis() const override;

        /**
         * \brief Set the synopsis of the movie.
         * \param synopsis New short description.
         */
        void set_synopsis(std::string synopsis) override;

    private:
        std::string _synopsis;
    };


//----------------------------------------------------
//               LAZY MOVIE CLASS
//----------------------------------------------------

    /**
     * \class LazyMovie
     * \brief Lightweight movie representation with external synopsis.
     *
     * Stores the main metadata of the movie but loads or stores
     * the synopsis in an external CSV file to reduce memory usage.
     */
    class LazyMovie: public Movie {
    public:
        /**
         * \brief Construct a lazy movie with minimal metadata.
         *
         * \param title Movie title
         * \param year Release year
         * \param producer Producer name
         * \param category Genre or category of the movie
         * \param cover Associated cover (normal/square)
         * \param director Director name
         * \param actors List of actors (string format)
         * \param csv_file Path to the CSV file containing the synopsis
         * \param duration Duration in minutes
         * \param video_file Filesystem path to the video file
         *
         * \note The CSV file must contain at least two columns: "title" and "synopsis".
         */
        LazyMovie(std::string title, int year, std::string producer,
            std::string category, Cover cover, std::string director,
            std::string actors, std::filesystem::path csv_file, int duration, 
            std::filesystem::path video_file);

        /**
         * \brief Get the synopsis of the movie from the CSV file.
         * \return Short description of the movie.
         * \throw std::runtime_error if the CSV file cannot be opened or
         * read/written.
         * 
         * \note This method performs a disk read each time it is called.
         *       For efficiency, avoid repeated calls. If multiple accesses are 
         *       expected, consider storing the result in a cache.
         */
        std::optional<std::string> synopsis() const override;

        /**
         * \brief Set the synopsis of the movie.
         * 
         * \param synopsis New short description.
         * \throw std::runtime_error if the CSV file cannot be opened or
         *        read/written.
         * 
         * \note The new synopsis is stored in the associated CSV file.
         *       This operation rewrites parts of the underlying CSV file and
         *       involves significant disk I/O. For efficiency, avoid calling
         *       this method repeatedly.
         */
        void set_synopsis(std::string synopsis) override;

    private:
        std::string _csv_file; ///< Path to the CSV file containing synopsis
    };

} // namespace core::data

#endif // FILM_H
