#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#include "interpolator.hpp"

int main()
{
	int width = 128;
	int height = 128;
	std::vector<std::vector<float>> specialFunctionTable;
	specialFunctionTable.resize(height);
	constexpr double halfPi = 3.1415926535 / 2.0;

#pragma omp parallel for
	for (int y = 0; y < height; ++y)
	{
		// v = [0, PI/2]
		double yStep = halfPi / static_cast<double>(width - 1);
		double v = y * yStep;
		specialFunctionTable[y].resize(width);
		for (int x = 0; x < width; ++x)
		{
			// u = [0, 10]
			double xStep = 10.0 / static_cast<double>(width - 1);
			double u = x * xStep;
			double equation1 = std::exp(-u * tan(v)) * v + (v / 8.0);
			double equation2 = 1 / (u + 0.55);
			specialFunctionTable[y][x] = Jormungandr::Lerp(equation1, equation2, y / static_cast<double>(height));

		}
	}

	std::vector<stbi_uc> writingPixels;
	writingPixels.reserve(width * height);
	uint32_t index = 0;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			index = i * height + j;
			writingPixels.push_back(255 * specialFunctionTable[i][j] / 2.0);

		}
	}

	std::string fileName = "specialFunction";
	std::string fullFilaName = "Output/" +  fileName + "_" + std::to_string(width) + "-" + std::to_string(height);
	std::string rootFileName = fullFilaName + ".png";
	int result = stbi_write_png(rootFileName.c_str(), width, height, 1, writingPixels.begin()._Ptr, width * 1);

	std::printf("\n\n -- result = %d \n\n", result);

	return 0;
}

	/*
	
	std::string path = "input/simple_hill.png";
	int texWidth;
	int texHeight;
	int texChannels;

	

	std::string fileName = "simple_hill_out";
	std::string fullFilaName = "output/" + fileName + "_";
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);



	std::printf("Image Info: %s\n", path.c_str());
	std::printf("Size: w:%d, h:%d\n", texWidth, texHeight);
	std::printf("Channels: %d\n", texChannels);
	uint32_t miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	std::printf("MipLevels: %d\n", miplevels);

	std::vector<stbi_uc> outPixels;
	outPixels.reserve(texWidth * texHeight);
	// create level 0 with 1 channel
	for (int i = 0; i < texWidth * texHeight; ++i)
	{
		//stbi_uc inverseColor = 
		outPixels.push_back(255 - pixels[i * texChannels]);
	}

	std::string rootFileName = fullFilaName + "0.png";
	stbi_write_png(rootFileName.c_str(), texWidth, texHeight, 1, outPixels.begin()._Ptr, texWidth * 1);

	stbi_image_free(pixels);

	for (int mip = 1; mip < miplevels; ++mip)
	{
		// read
		std::printf("---------------------------------\n");

		std::string pathIn = fullFilaName + std::to_string(mip - 1) + ".png";
		stbi_uc* pixels = stbi_load(pathIn.c_str(), &texWidth, &texHeight, &texChannels, STBI_grey);
		std::printf("Image Info: %s\n", pathIn.c_str());
		std::printf("Size: w:%d, h:%d\n", texWidth, texHeight);
		std::printf("Channels: %d\n", texChannels);

		std::vector<stbi_uc> outPixels;
		outPixels.reserve((texWidth / 2) * (texHeight / 2));
		int width = texWidth / 2;
		int height = texHeight / 2;

#pragma omp parallel 
		for (int i = 0; i < texHeight / 2; ++i)
		{
			for (int j = 0; j < texWidth / 2; ++j)
			{
				stbi_uc topLeft = pixels[(j * 2 + i * 2 * width * 2) * texChannels];
				stbi_uc topRight = pixels[((j * 2 + 1) + i * 2 * width * 2) * texChannels];
				stbi_uc bottomLeft = pixels[(j * 2 + (i * 2 + 1) * width * 2) * texChannels];
				stbi_uc bottomRight = pixels[((j * 2 + 1) + (i * 2 + 1) * width * 2) * texChannels];

				stbi_uc max = std::min(std::min(topLeft, topRight), std::min(bottomLeft, bottomRight));

				outPixels.push_back(max);
			}

		}
		// write
		std::string path = fullFilaName + std::to_string(mip) + ".png";
		std::printf("test path: %s\n", path.c_str());
		stbi_write_png(path.c_str(), width, height, 1, outPixels.begin()._Ptr, width * 1);

		stbi_image_free(pixels);
	}
	*/