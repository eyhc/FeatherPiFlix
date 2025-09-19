#ifndef IMAGE_FORMAT_H
#define IMAGE_FORMAT_H

#include <string>
#include <gd.h>
#include <filesystem>

#include "cover.h"

/**
 * \file image_format.h
 * \brief Image formatting utilities for generating movie covers.
 *
 * Provides functions to generate standard and square covers,
 * and to resize or scale images using the GD library.
 *
 * Rules :
 * - Standard cover: width = 240px, height = 320px
 * - Square cover: width = 240px, height = 240px
 *
 * \note The square image is stretched to fit dimensions.
 */

namespace core::image_format {

    /**
     * \brief Create standard and square cover images from a source.
     *
     * Generates two images:
     * - a standard cover (240x320)
     * - a square cover (240x240)
     *
     * \param img_source Path to the source image file.
     * \param output_name Base name for generated files.
     * \param output_dir Directory where images will be saved.
     * \return A Cover object referencing the generated images.
     */
    data::Cover create_covers(
        std::string img_source,
        std::string output_name,
        std::filesystem::path output_dir
    );

    /**
     * \brief Resize an image proportionally to a given width.
     * \param image Pointer to the GD image to resize.
     * \param width Target width in pixels.
     * \return A new GD image pointer resized proportionally.
     */
    gdImagePtr resize_to_width(gdImagePtr image, int width);

    /**
     * \brief Scale an image by a ratio.
     * \param image Pointer to the GD image to scale.
     * \param ratio Scaling factor (e.g. 0.5 = half size).
     * \return A new GD image pointer scaled by the ratio.
     */
    gdImagePtr scale_image(gdImagePtr image, double ratio);

    /**
     * \brief Resize an image to exact dimensions (may distort).
     * \param src Pointer to the GD source image.
     * \param width Target width in pixels.
     * \param height Target height in pixels.
     * \return A new GD image pointer resized to exact size.
     */
    gdImagePtr resize_exact(gdImagePtr src, int width, int height);
} // namespace core::image_format

#endif // IMAGE_FORMAT_H
