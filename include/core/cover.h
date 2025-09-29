#ifndef COVER_H
#define COVER_H

#include <string>
#include <filesystem>

/**
 * \file cover.h
 * \brief Defines the Cover class representing images associated with a movie.
 */

namespace core::data {

    /**
     * \brief Represents an image associated with a movie.
     *
     * An image is defined by two paths:
     * - a "normal" format (e.g., standard poster or cover)
     * - a "square" format (e.g., thumbnail or avatar style)
     */
    class Cover {
    public:
        /**
         * \brief Default constructor.
         * Creates an empty image with default paths set.
         */
        Cover();

        /**
         * \brief Construct an Image with the given paths.
         *
         * \param normal_path Path to the normal/standard image.
         * \param square_path Path to the square format image.
         */
        Cover(
            const std::filesystem::path &normal_path,
            const std::filesystem::path &square_path
        );

        /**
         * \brief Get the path of the normal image.
         * \return The filesystem path of the normal image.
         */
        std::filesystem::path normal_path() const;

        /**
         * \brief Get the path of the square image.
         * \return The filesystem path of the square image.
         */
        std::filesystem::path square_path() const;

        /**
         * \brief Set the path of the normal image.
         * \param path New filesystem path to use for the normal image.
         */
        void set_normal_path(const std::filesystem::path &path);

        /**
         * \brief Set the path of the square image.
         * \param path New filesystem path to use for the square image.
         */
        void set_square_path(const std::filesystem::path &path);

        /**
         * \brief Convert the image information to a string.
         * \return A string representation of the image paths.
         */
        std::string to_string() const;

    private:
        std::filesystem::path _normal; ///< Path to the normal format image
        std::filesystem::path _square; ///< Path to the square format image
    };

} // namespace core::data

#endif // COVER_H
