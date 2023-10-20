#pragma once
#include <GL/glew.h>
#include <SDL.h>
//#include <SDL_image.h>
//#include "stb_image.h"
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
namespace RGLib {

	class Texture
	{
	public:
		Texture(std::string filename)
		{
			width = 0;
			height = 0;

			textureName = 0;

			loadTexture(filename);
		}
	

		~Texture();

		//accessor methods
		int getWidth()
		{
			return width;
		}
		int getHeight()
		{
			return height;
		}

		int getBytesPerPixel()
		{
			return bytesPerPixel;
		}

		Uint32 getFormat()
		{
			return format;
		}

		GLuint getTextureName()
		{
			return  textureName;
		}

		unsigned char* getPixelData()
		{
			return pixelData;
		}


	private:
		void loadTexture(std::string filename);

	private:

		int width;
		int height;

		int bytesPerPixel;

		unsigned char* pixelData;

		Uint32 format;
		SDL_Surface* surfaceImage;
		GLuint textureName;
	};
}


