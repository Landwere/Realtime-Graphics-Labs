#pragma once
#include <Eigen/Dense>
namespace RGLib
{
	class Light
	{
	public:
		Light();
		Light(float intensity, Eigen::Vector3f pos)
		{
			lightIntensity = intensity;
			lightPos = pos;
		}

		void UpdatePos(Eigen::Vector3f newPos)
		{
			lightPos = newPos;
		}

		float getX()
		{
			return lightPos.x();
		}
		float getY()
		{
			return lightPos.y();
		}
		float getZ()
		{
			return lightPos.z();
		}

		Eigen::Vector3f GetPos()
		{
			return lightPos;
		}

		float Intensity()
		{
			return lightIntensity;
		}

		void setIntensity(float intensity)
		{
			lightIntensity = intensity;
		}


		float lightWidth;
		float lightRadius;
	private:
		Eigen::Vector3f lightPos;
		float lightIntensity;

	};
}