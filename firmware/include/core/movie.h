#ifndef FILM_H
#define FILM_H

#include <string>
#include <filesystem>
#include <optional>
#include <functional>

#include "cover.h"

/**
 * \file movie.h
 * \brief Defines abstract and concrete movie classes with metadata management.
 */

namespace core::data {
    /*---------------------------------------------------------
                        Synopsis providers
      ---------------------------------------------------------*/


    /**
     * \brief Abstract interface for providing a movie synopsis.
     *
     * This allows different strategies to fetch or store a synopsis
     * (direct string, CSV file, database, etc.).
     */
    class SynopsisProvider {
    public:
        /**
         * \brief Get the synopsis text.
         * \return Synopsis string.
         * \throws std::runtime_error If an error occurs during reading.
         */
        virtual std::string get_synopsis() const = 0;


        /**
         * \brief Set or update the synopsis text.
         * \param synopsis New synopsis text.
         * \throws std::runtime_error If an error occurs during editing.
         */
        virtual void set_synopsis(const std::string &synopsis) = 0;

        /// Virtual destructor for safe polymorphic deletion.
        virtual ~SynopsisProvider() = default;
    };

    /**
     * \brief Provides a synopsis stored directly as a string.
     */
    class DirectSynopsisProvider: public SynopsisProvider {
    public:
        /**
         * \brief Construct with an initial synopsis.
         * \param synopsis Initial synopsis string.
         */
        DirectSynopsisProvider(const std::string &synopsis);

        /**
         * \brief Return the stored synopsis string.
         * \return Stored synopsis.
         */
        virtual std::string get_synopsis() const override;

        /**
         * \brief Update the stored synopsis string.
         * \param synopsis New synopsis text.
         */
        virtual void set_synopsis(const std::string &synopsis) override;
    
    private:
        std::string _synopsis; ///< Stored synopsis text
    };

    /**
     * \brief Provides a synopsis stored in a CSV file.
     *
     * The synopsis is retrieved by matching the movie title in the CSV file.
     * \note The CSV file must contain at least two columns named
     * "title" and "synopsis".
     */
    class CSVFileSynopsisProvider: public SynopsisProvider {
    public:
        /**
         * \brief Construct with the movie title and CSV filename.
         * \param movie_title Title of the movie to look up.
         * \param csv_filename Path to the CSV file.
         */
        CSVFileSynopsisProvider(
            const std::string &movie_title, const std::string &csv_filename);

        /**
         * \brief Retrieve the synopsis from the CSV file.
         * \return Synopsis string from the CSV file.
         * 
         * \throws std::runtime_error If the CSV file cannot be opened 
         *         for reading.
         */
        virtual std::string get_synopsis() const override;

        /**
         * \brief Update the synopsis in the CSV file.
         * \param synopsis New synopsis text.
         * \throws std::runtime_error If the CSV file cannot be opened 
         *         for reading 
         * \throws std::runtime_error If the temporary file cannot be opened 
         *         for writing.
         * \throws std::runtime_error If an error occurs while rewriting 
         *         the CSV file.
         * \note Overwrite all the CSV file
         */
        virtual void set_synopsis(const std::string &synopsis) override;
    private:
        const std::string _title;      ///< Movie title used for lookup
        const std::string _csv_file;   ///< Path to the CSV file
    };

    /*---------------------------------------------------------
                            Movie class
      ---------------------------------------------------------*/

    /**
     * \brief Represents a movie with metadata and a synopsis provider.
     *
     * A movie stores information such as title, year, category,
     * producer, director, actors, duration, cover, and video file path.
     * The synopsis is managed by a pluggable provider.
     */
    class Movie {
    public:
        /**
         * \brief Construct a Movie with a custom synopsis provider.
         * 
         * \param title Movie title.
         * \param year Release year.
         * \param category Genre or category.
         * \param producer Producer name.
         * \param director Director name.
         * \param actors List of actors in string format.
         * \param duration Duration in minutes.
         * \param synopsis Custom synopsis provider.
         * \param cover Associated cover object.
         * \param video_file Filesystem path to the video file.
         */
        Movie(
            const std::string &title,
            int year,
            const std::string &category,
            const std::string &producer,
            const std::string &director,
            const std::string &actors,
            int duration,
            std::unique_ptr<SynopsisProvider> synopsis, 
            const Cover &cover,
            const std::filesystem::path &video_file
        );

        /**
         * \brief Construct a Movie with a direct synopsis string.
         * \param title Movie title.
         * \param year Release year.
         * \param category Genre or category.
         * \param producer Producer name.
         * \param director Director name.
         * \param actors List of actors in string format.
         * \param duration Duration in minutes.
         * \param synopsis Synopsis string.
         * \param cover Associated cover object.
         * \param video_file Filesystem path to the video file.
         * 
         * \note this constructor uses a \c DirectSynopsisProvider
         */
        Movie(
            const std::string &title,
            int year,
            const std::string &category,
            const std::string &producer,
            const std::string &director,
            const std::string &actors,
            int duration,
            const std::string &synopsis, 
            const Cover &cover,
            const std::filesystem::path &video_file
        );

        
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
         * \brief Get the category/genre of the movie.
         * \return Category name.
         */
        std::string category() const;

        /**
         * \brief Get the producer of the movie.
         * \return Producer name.
         */
        std::string producer() const;

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
         * \brief Get the duration of the movie in minutes.
         * \return Duration in minutes.
         */
        int duration() const;

        /**
         * \brief Get the synopsis of the movie.
         * \return Short description of the movie.
         * \throws std::runtime_error If an error occurs during reading.
         * \note Just calls the synopsis provider.
         */
        std::string synopsis() const;

        /**
         * \brief Get duration formatted as "Hh Mmin".
         * \return Human-readable duration string.
         */
        std::string duration_str() const;

        /**
         * \brief Get the cover of the movie.
         * \return Cover object.
         */
        Cover cover() const;

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
        void set_year(int year);

        /**
         * \brief Set the category/genre of the movie.
         * \param category New category.
         */
        void set_category(const std::string &category);

        /**
         * \brief Set the producer of the movie.
         * \param producer New producer name.
         */
        void set_producer(const std::string &producer);

        /**
         * \brief Set the director of the movie.
         * \param director New director name.
         */
        void set_director(const std::string &director);

        /**
         * \brief Set the actors of the movie.
         * \param actors New list of actors (string format).
         */
        void set_actors(const std::string &actors);

        /**
         * \brief Set the duration of the movie in minutes.
         * \param duration Duration in minutes.
         */
        void set_duration(int duration);

        /**
         * \brief Set the synopsis of the movie.
         * \param synopsis New short description.
         * \throws std::runtime_error If an error occurs during editing.
         * \note Just calls the synopsis provider.
         */
        void set_synopsis(const std::string &synopsis);

        /**
         * \brief Set the cover of the movie.
         * \param cover New cover object.
         */
        void set_cover(const Cover &cover);

        /**
         * \brief Set the video file path of the movie.
         * \param path New filesystem path to the video file.
         * 
         * \note No warranty is provided regarding the existence of the file.
         */
        void set_video_file(const std::filesystem::path &path);


        // Synopsis Provider

        /**
         * \brief Change the synopsis provider using a transformation function.
         *
         * Transfers ownership of the current synopsis provider to \p chg_fct,
         * then stores the provider returned by the function.
         *
         * \param chg_fct Function taking the current synopsis provider
         *        (as a unique_ptr) and returning a new provider (as a unique_ptr).
         *
         * \note The current provider is moved into \p chg_fct. The returned provider
         * replaces it. So, the given \c chg_fct function MUST never return nullptr.
         */
        void change_synopsis_provider(std::function<
            std::unique_ptr<data::SynopsisProvider>(
                std::unique_ptr<data::SynopsisProvider>
            )> chg_fct);

        /**
         * \brief Get the current synopsis provider.
         * 
         * Returns the underlying provider used to retrieve and update the
         * synopsis for this movie.
         * 
         * \return Reference to the synopsis provider.
         */
        std::reference_wrapper<data::SynopsisProvider> 
            get_synopsis_provider() const;


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
        std::string _title;                ///< Movie title
        int _year;                         ///< Release year
        std::string _category;             ///< Movie category/genre
        std::string _producer;             ///< Producer name
        std::string _director;             ///< Director name
        std::string _actors;               ///< Actors list in string format
        int _duration;                     ///< Duration in minutes
        /// Custom synopsis provider
        std::unique_ptr<SynopsisProvider> _synopsis;
        Cover _cover;                      ///< Associated cover
        std::filesystem::path _video_file; ///< Video file path
    };

    /// Alias for a reference to a Movie object.
    typedef std::reference_wrapper<data::Movie> movie_ref;

} // namespace core::data

#endif // FILM_H
