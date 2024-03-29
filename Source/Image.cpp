/*
 * Image.cpp
 *
 *  Created on: 2 Jul 2014
 *      Author: Dimitri Kourkoulis
 */

#include "Image.h"

#include <stdio.h>
#include <boost/numeric/conversion/cast.hpp>
#include "GameException.h"

using namespace std;

namespace AvoidTheBug3D
{

	Image::Image(const string &fileLocation, const boost::shared_ptr<Configuration> cfg,
		const boost::shared_ptr<GameLog> log)
{
    this->cfg = cfg;
    this->log = log;

    width = 0;
    height = 0;

    imageData = NULL;

    this->loadFromFile(fileLocation);

}

Image::~Image()
{

    if (imageData != NULL)
    {
        delete[] imageData;
    }
}

void Image::loadFromFile(const string &fileLocation)
{
    // function developed based on example at
    // http://zarb.org/~gc/html/libpng.html
#ifdef _WIN32
    FILE *fp;
    fopen_s(&fp, (cfg->getHomeDirectory() + fileLocation).c_str(), "rb");
#else
    FILE *fp = fopen((cfg->getHomeDirectory() + fileLocation).c_str(), "rb");
#endif
    if (!fp)
    {
        throw GameException(
            "Could not open file " + cfg->getHomeDirectory()
            + fileLocation);
    }

    png_infop pngInformation = NULL;
    png_structp pngStructure = NULL;
    png_byte colorType = (png_byte) 0;
    png_byte bitDepth = (png_byte) 0;
    int numberOfPasses = 0;
    png_bytep* rowPointers = NULL;

    unsigned char header[8]; // Using maximum size that can be checked

    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        throw GameException(
            "File " + cfg->getHomeDirectory() + fileLocation
            + " is not recognised as a PNG file.");
    }

    pngStructure = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                          NULL, NULL, NULL);

    if (!pngStructure)
    {
        fclose(fp);
        throw GameException("Could not create PNG read structure.");
    }

    pngInformation = png_create_info_struct(pngStructure);

    if (!pngInformation)
    {
        png_destroy_read_struct(&pngStructure, NULL, NULL);
        fclose(fp);
        throw GameException("Could not create PNG information structure.");
    }

    if (setjmp(png_jmpbuf(pngStructure)))
    {
        png_destroy_read_struct(&pngStructure, &pngInformation, NULL);
        pngStructure = NULL;
        pngInformation = NULL;
        fclose(fp);
        throw GameException("PNG read: Error calling setjmp. (1)");
    }

    png_init_io(pngStructure, fp);
    png_set_sig_bytes(pngStructure, 8);

    png_read_info(pngStructure, pngInformation);

    width = png_get_image_width(pngStructure, pngInformation);
    height = png_get_image_height(pngStructure, pngInformation);
    colorType = png_get_color_type(pngStructure, pngInformation);
    bitDepth = png_get_bit_depth(pngStructure, pngInformation);

    numberOfPasses = png_set_interlace_handling(pngStructure);
    png_read_update_info(pngStructure, pngInformation);

    if (setjmp(png_jmpbuf(pngStructure)))
    {
        png_destroy_read_struct(&pngStructure, &pngInformation, NULL);
        pngStructure = NULL;
        pngInformation = NULL;
        fclose(fp);
        throw GameException("PNG read: Error calling setjmp. (2)");
    }

    rowPointers = new png_bytep[sizeof(png_bytep) * height];

    for (int y = 0; y < height; y++)
    {
        rowPointers[y] = new png_byte[png_get_rowbytes(pngStructure,
                                      pngInformation)];
    }

    png_read_image(pngStructure, rowPointers);

    if (png_get_color_type(pngStructure, pngInformation) != PNG_COLOR_TYPE_RGB)
    {
        throw GameException(
            "For now, only PNG_COLOR_TYPE_RGB is supported for PNG images.");
    }

    imageData = new float[4 * width * height];

    for (int y = 0; y < height; y++)
    {

        png_byte* row = rowPointers[y];

        for (int x = 0; x < width; x++)
        {

            png_byte* ptr = &(row[x * 3]);

            float rgb[3];

            rgb[0] = boost::numeric_cast<float, png_byte>(ptr[0]) ;
            rgb[1] = boost::numeric_cast<float, png_byte>(ptr[1]) ;
            rgb[2] = boost::numeric_cast<float, png_byte>(ptr[2]) ;

            imageData[y * width * 4 + x * 4] =  floorf(100.0f * (rgb[0] / 255.0f) + 0.5f) / 100.0f;
            imageData[y * width * 4 + x * 4 + 1] = floorf(100.0f * (rgb[1] / 255.0f) + 0.5f) / 100.0f;
            imageData[y * width * 4 + x * 4 + 2] = floorf(100.0f * (rgb[2] / 255.0f) + 0.5f) / 100.0f;
            imageData[y * width * 4 + x * 4 + 3] = 1.0f;

        }
    }

    fclose(fp);

    if (rowPointers != NULL)
    {
        for (int y = 0; y < height; y++)
        {
            delete[] rowPointers[y];
        }
        delete[] rowPointers;
    }

    if (pngInformation != NULL || pngStructure != NULL)
    {
        png_destroy_read_struct(&pngStructure, &pngInformation, NULL);
        pngStructure = NULL;
        pngInformation = NULL;
    }
}

int Image::getWidth() const
{
    return width;
}

int Image::getHeight() const
{
    return height;
}

float *Image::getData() const
{
    return imageData;
}

}

