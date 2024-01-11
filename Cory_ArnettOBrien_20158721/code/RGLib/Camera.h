#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL_mouse.h>

namespace RGLib
{
	/*! \brief Unused in favour of glhelper::FlyViewer 
	*
	* Source Cory Arnett-O'Brien (Game Engine Architecture)
	*/
	class Camera
	{
	public:
		Camera(glm::vec3 cam_pos, glm::vec3 cam_target, glm::vec3 cam_upDir, float fov, float ar, float near, float far)
		{
			//initialise the camera variables to the values passed on the parameters
			pos = cam_pos;
			target = cam_target;
			up = cam_upDir;

			fovy = fov;
			aspectR = ar;
			nearClip = near;
			farClip = far;

			updateCamMatrices();
			//SDL_CaptureMouse(SDL_TRUE);

			SDL_GetMouseState(&oldMouseX, &oldMouseY);
		}

		float getPosX()
		{
			return pos.x;
		}
			float getPosY()
		{
			return pos.y;
		}
		float getPosZ()
		{
			return pos.z;
		}

		glm::vec3 getPos()
		{
			return pos;
		}

		glm::vec3 getTarget()
		{
			return target;
		}

		glm::vec3 getUpDir()
		{
			return up;
		}

		float getPitch()
		{
			return pitch;
		}

		float getYaw()
		{
			return yaw;
		}

		float getOldMouseX()
		{
			return oldMouseX;
		}

		float getOldMouseY()
		{
			return oldMouseY;
		}

		//return the camera view matrix. Used by draw
		//method to send view matrix to vertex shader
		glm::mat4 getViewMatrix()
		{
			return viewMat;
		}
		//see above 
		glm::mat4 getProjectionMatrix()
		{
			return projectionMat;
		}


		//set positions
		void setPosX(float newX)
		{
			pos = glm::vec3(newX, pos.y, pos.z);

				updateCamMatrices();
		}

		void setPosY(float newY)
		{
			pos = glm::vec3(pos.x, newY, pos.z);
			updateCamMatrices();
		}

		void setPosZ(float newZ)
		{
			pos = glm::vec3(pos.x, pos.y, newZ);

			updateCamMatrices();
		}

		//set position of all axes in one method
		void setPos(glm::vec3 newPos)
		{
			pos = newPos;

			updateCamMatrices();
		}

		//set new target
		void setTarget(glm::vec3 newTarget)
		{
			target = newTarget;

			updateCamMatrices();
		}

		//set the up driection
		void setUpDir(glm::vec3 newUp)
		{
			up = newUp;

			updateCamMatrices();
		}

		//set the fov
		void setFov(float newFov)
		{
			fovy = newFov;

			updateCamMatrices();
		}

		//set the aspect ratio
		//I might use this mthod if the programs winodw
		//can be resized meaning the width and height change
		void setAspectRatio(float newAR)
		{
			aspectR = newAR;

			updateCamMatrices();
		}

		//set clipping planes
		void setNearClip(float newNearClip)
		{
			nearClip = newNearClip;

			updateCamMatrices();

		}

		void setFarClip(float newFarClip)
		{
			farClip = newFarClip;
			
			updateCamMatrices();

		}
		~Camera() {};


		void setPitch(float newPitch)
		{
			pitch = newPitch;

			//clamp values so we cant look directly up or down
			if (pitch > 70.0f) pitch = 70.0f;
			if (pitch < -70.0f) pitch = -70.f;
		}

		void setYaw(float newYaw)
		{
			yaw = newYaw;
		}

		void setOldMouseX(float newOldMouseX)
		{
			oldMouseX = newOldMouseX;
		}

		void setOldMouseY(float newOldMouseY)
		{
			oldMouseY = newOldMouseY;
		}

		//updates the camera matrices done in
	//response to a member variable being updated
		void updateCamMatrices()
		{
			viewMat = glm::lookAt(pos, pos + target, up);
			projectionMat = glm::perspective(glm::radians(fovy), aspectR, nearClip, farClip);
		}

		private:
	

			//member variables
			//camera view variables
			glm::vec3 pos;
			glm::vec3 target;
			glm::vec3 up;

			//camera projection variables
			float fovy;
			float aspectR;
			float nearClip;
			float farClip;

			float pitch = 0.0f;
			float yaw = -90.0f;

			int oldMouseX, oldMouseY;

			//view and projection matrices
			glm::mat4 viewMat;
			glm::mat4 projectionMat;
	};
	//end source
}


