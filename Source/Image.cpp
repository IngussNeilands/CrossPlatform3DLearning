/*
 * Image.cpp
 *
 *  Created on: 2 Jul 2014
 *      Author: Dimitri Kourkoulis
 */

#include "Image.h"

#include <stdio.h>
#include "GameException.h"

using namespace std;

namespace AvoidTheBug3D {

Image::Image(string fileLocation, const boost::shared_ptr<Configuration>& cfg,
		const boost::shared_ptr<GameLog>& log) {
	this->cfg = cfg;
	this->log = log;

	pngInformation = NULL;
	pngStructure = NULL;

	width = 0;
	height = 0;
	colorType = (png_byte) 0;
	bitDepth = (png_byte) 0;
	numberOfPasses = 0;

	rowPointers = NULL;

	imageData = NULL;

	this->loadFromFile(fileLocation);

}

Image::~Image() {

	if (rowPointers != NULL) {
		for (int y = 0; y < height; y++) {
			delete[] rowPointers[y];
		}
		delete[] rowPointers;
	}

	if (pngInformation != NULL) {
		delete pngInformation;
	}

	if (pngStructure != NULL) {
		delete pngStructure;
	}

	if (imageData != NULL) {
		delete[] imageData;
	}
}

void Image::loadFromFile(string fileLocation) {
	// function developed based on example at
	// http://zarb.org/~gc/html/libpng.html

	FILE *fp = fopen((cfg->getHomeDirectory() + fileLocation).c_str(), "rb");

	if (!fp) {
		throw GameException(
				"Could not open file " + cfg->getHomeDirectory()
						+ fileLocation);
	}

	unsigned char header[8]; // Using maximum size that can be checked

	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8)) {
		throw GameException(
				"File " + cfg->getHomeDirectory() + fileLocation
						+ " is not recognised as a PNG file.");
	}

	pngStructure = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	NULL, NULL, NULL);

	if (!pngStructure) {
		delete pngStructure;
		fclose(fp);
		throw GameException("Could not create PNG read structure.");
	}

	pngInformation = png_create_info_struct(pngStructure);

	if (!pngInformation) {
		fclose(fp);
		throw GameException("Could not create PNG information structure.");
	}

	if (setjmp(png_jmpbuf(pngStructure))) {
		delete pngInformation;
		delete pngStructure;
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

	if (setjmp(png_jmpbuf(pngStructure))) {
		delete pngInformation;
		delete pngStructure;
		fclose(fp);
		throw GameException("PNG read: Error calling setjmp. (2)");
	}

	rowPointers = new png_bytep[sizeof(png_bytep) * height];

	for (int y = 0; y < height; y++) {
		rowPointers[y] = new png_byte[png_get_rowbytes(pngStructure,
				pngInformation)];
	}

	png_read_image(pngStructure, rowPointers);

	if (png_get_color_type(pngStructure, pngInformation) != PNG_COLOR_TYPE_RGB) {
		throw GameException(
				"For now, only PNG_COLOR_TYPE_RGB is supported for PNG images.");
	}

	imageData = new unsigned short[width * height * 3];

	for (int y = 0; y < height; y++) {

		png_byte* row = rowPointers[y];

		for (int x = 0; x < width; x++) {

			png_byte* ptr = &(row[x * 3]);
			imageData[y * width * 3 + x * 3] = ptr[0];
			imageData[y * width * 3 + x * 3 + 1] = ptr[1];
			imageData[y * width * 3 + x * 3 + 2] = ptr[2];

		}
	}

	fclose(fp);
}

int Image::getWidth() const {
	return width;
}

int Image::getHeight() const {
	return height;
}

unsigned short *Image::getData() const {
	return imageData;
}

}

