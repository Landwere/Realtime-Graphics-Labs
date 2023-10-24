#include "Texture.h"

namespace RGLib {
	void Texture::loadTexture(std::string filename)
	{
		//IMG_Init(IMG_INIT_JPG);
		//load texture from file
		//surfaceImage = IMG_Load(filename.c_str());
		//int height, width, channels;
		cv::Mat mat = cv::imread(filename);
		unsigned char* image = mat.data; //  stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (image == nullptr)
		{
			std::cerr << "failed to load surface image!" << " " << filename.c_str() << std::endl;
			return;
		}
			std::cerr << filename.c_str() <<" Loaded correctly!" << std::endl;
		//check texture loaded correctly
		//if (surfaceImage == nullptr)
		//{
		//	std::cerr << "failed to load surface image!" << " " << filename.c_str() << std::endl;
		//	return;
		//}
		//std::cerr << filename.c_str() << std::endl;

		////get the dimentions needed for OpenGL
		//width = surfaceImage->w;
		//height = surfaceImage->h;

		//bytesPerPixel = surfaceImage->format->BytesPerPixel;

		//pixelData = (unsigned char*)surfaceImage->pixels;

		////get the format from the SDL object, will be in SDL Image's format
		//format = surfaceImage->format->format;

		////determine OpenGL format from SDL format 
		//switch (format)
		//{
		//	//RGBA where there are 8 bits per pixel, 32 bits total
		//case SDL_PIXELFORMAT_RGBA32: format = GL_RGBA;
		//		break;
		//	//RGB 8 bits per pixel, 24 bits total
		//case SDL_PIXELFORMAT_RGB24: format = GL_RGB;
		//	break;

		//	//potential to add for formats here

		//	//defualt RGB
		//default:
		//	format = GL_RGB;
		//	break;
		//}
		/*if (SDL_MUSTLOCK(surfaceImage))
		{
			SDL_LockSurface(surfaceImage);
		}*/
		//create a texutre name for the texture 
		glGenTextures(1, &textureName);

		//select created texture for sebsequent texture operations 
		//to setup the texture for OpenGL
		glBindTexture(GL_TEXTURE_2D, textureName);

		//copy the pixel data from the SDL_Surface object 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		//configure how the texutre will be manipulated when it needs 
		//to be reduced or increased (magnified) when rendering onto an object
		//GL_LINEAR is a weighted average of the colours around the texture co-ords
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//release the surface from memory
		//SDL_FreeSurface(surfaceImage);
		//stbi_image_free(image);
		//surfaceImage = NULL;
		return;
	}

	Texture::~Texture() {
		glDeleteTextures(1, &textureName);
	}
}