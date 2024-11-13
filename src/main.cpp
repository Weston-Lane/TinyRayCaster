#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <assert.h>
#include <sstream>
#include <iomanip>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const size_t winH = 512;
const size_t winW = 1024;

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

void dropImage(std::vector<uint32_t>& image, std::string name)
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

	stbi_write_jpg(name.c_str(), winW, winH, 3, imageData.data(), 100);
}

void drawRectangle(std::vector<uint32_t>& img, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color )
{
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			size_t cx = x + i;
			size_t cy = y + j;
			if (cx >= winW || cy >= winH)
				continue;
			img[cx + cy * winW] = color;
		}
	}
}

int main()
{
	size_t mapW = 16; //map is 16x16 rectangles
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

	std::vector<uint32_t> frameBuffer(winH*winW,packColor(255,255,255));//initializes all pixels to white

	//sets random colors for each wall value
	size_t nColors=10;
	std::vector<uint32_t> colors(nColors);
	for (int i=0; i < nColors; i++)
		colors[i] = packColor(rand() % 255, rand() % 255, rand() % 255);

	//fills in frame buffer with gradient//refactored
	//for (size_t i = 0; i < winH; i++)
	//{
	//	for (size_t j = 0; j < winW; j++)
	//	{
	//		uint8_t r = 255 * j / winH;
	//		uint8_t g = 255 * i / winW;
	//		uint8_t b = 0;
	//		frameBuffer[i + j * winW] = packColor(r, g, b);
	//	}
	//}


	size_t recW = winW/(mapW*2); //rectangles are 32x32 pixels each
	size_t recH = winH / mapH;
	//jpg for each frame
	float playerViewDir = 1.523;
	for (size_t frame = 0; frame < 360; frame++)
	{
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(5) << frame << ".jpg";
		playerViewDir += 2 * M_PI / 360;
		//clear screen
		frameBuffer = std::vector<uint32_t>(winW * winH, packColor(255, 255, 255));

		for (size_t i = 0; i < mapW; i++)
		{
			for (size_t j = 0; j < mapH; j++)
			{
				if (map[i + j * mapW] == ' ')
					continue;

				size_t recX = recW * i;
				size_t recY = recH * j;
				//subtract the character '0' which makes the value of the map an int value
				size_t iColor = map[i + j * mapW] - '0';
				drawRectangle(frameBuffer, recX, recY, recW, recH, colors[iColor]);
			}
		}


		//place player in rec coords
		float playerX = 3.456;
		float playerY = 2.345;
		const float fov = 60; //in degrees
		const float fovRad = fov * M_PI / 180;//in radians
		//draws player at 3.4x and 2.3y in rec coords and makes it 5x5 pixels
		drawRectangle(frameBuffer, playerX * recW, playerY * recH, 5, 5, packColor(255, 255, 255));




		for (size_t i = 0; i < winW / 2; i++)
		{
			float angle = (playerViewDir - (fovRad / 2)) + ((fovRad * i) / float(winW / 2));
			//cast ray
			for (float t = 0; t < 20; t += .01f)
			{
				float cx = playerX + t * cos(angle);
				float cy = playerY + t * sin(angle);

				size_t pixX = cx * recW;
				size_t pixY = cy * recH;

				frameBuffer[(pixX)+((pixY)*winW)] = packColor(160, 160, 160);

				//draws verticle wall
				if (map[(int)cx + (int)cy * mapW] != ' ')
				{
					size_t iColor = map[(int)cx + (int)cy * mapW] - '0';//subtracting the char '0' changes the map[] value to be an int
					size_t columnHeight = winH / (t*cos(angle-playerViewDir));//removes fisheye effect
					drawRectangle(frameBuffer, (winW / 2) + i, (winH / 2) - (columnHeight / 2), 1, columnHeight, colors[iColor]);
					break;
				}

			}
		}

		//create image
		dropImage(frameBuffer,ss.str());
	}

	return 0;
}