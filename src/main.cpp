#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const size_t winH = 512;
const size_t winW = 512;

uint32_t packColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
	return (a << 24) + (b << 16) + (g << 8) + (r << 0);
}

void unpackColor(const uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
	r = (color >> 0) & 255;
	g = (color >> 8) & 255;
	b = (color >> 16) & 255;
	a = (color >> 24) & 255;
}

void dropImage(std::vector<uint32_t>& image)
{
	std::vector<unsigned char> imageData(winW * winH * 3,255);
	for (size_t i = 0; i < winW * winH; ++i)
	{
		uint8_t r, g, b,a;
		unpackColor(image[i], r, g, b, a);
		imageData[i * 3] = r;
		imageData[i * 3 + 1] = g;
		imageData[i * 3 + 2] = b;
		//imageData[i * 4 + 3] = (unsigned char)a;
	}

	stbi_write_jpg("out.jpg", winW, winH, 3, imageData.data(), 100);
}

void drawRectangle(std::vector<uint32_t>& img, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color )
{
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			size_t cx = x + i;
			size_t cy = y + j;

			img[cx + cy * winW] = color;
		}
	}
}

int main()
{
	size_t mapW = 16;
	size_t mapH = 16;

	const char map[]= "0000222222220000"\
					  "1              0"\
					  "1      11111   0"\
					  "1     0        0"\
					  "0     0  1110000"\
					  "0     3        0"\
					  "0   10000      0"\
					  "0   0   11100  0"\
					  "0   0   0      0"\
					  "0   0   1  00000"\
					  "0       1      0"\
					  "2       1      0"\
					  "0       0      0"\
					  "0 0000000      0"\
					  "0              0"\
					  "0002222222200000";

	assert(sizeof(map) == mapW * mapH + 1);

	std::vector<uint32_t> frameBuffer(winH*winW,255);//initializes all pixels to red

	for (size_t i = 0; i < winH; i++)
	{
		for (size_t j = 0; j < winW; j++)
		{
			uint8_t r = 255 * j / winH;
			uint8_t g = 255 * i / winW;
			uint8_t b = 0;
			frameBuffer[i + j * winW] = packColor(r, g, b);
		}
	}


	size_t recW = winW/mapW;
	size_t recH = winH / mapH;

	for (size_t i = 0; i < mapW; i++)
	{
		for (size_t j = 0; j < mapH; j++)
		{
			if (map[i + j * mapW] == ' ')
				continue;

			size_t recX = recW * i;
			size_t recY = recH * j;

			drawRectangle(frameBuffer, recX, recY, recW, recH, packColor(0, 255, 255));
		}
	}



	dropImage(frameBuffer);

	return 0;
}