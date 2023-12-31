#include "TextureLegacy.h"

namespace RGLib {
	//source Cory Arnett-O'Brien (Game engine architecture) 
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

		//Create mip maps
		glGenerateMipmap(textureName);


		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);

		//release the surface from memory
		//SDL_FreeSurface(surfaceImage);
		//stbi_image_free(image);
		//surfaceImage = NULL;
		return;
	}

	Texture::~Texture() {
		glDeleteTextures(1, &textureName);
	}
	//end sources
}