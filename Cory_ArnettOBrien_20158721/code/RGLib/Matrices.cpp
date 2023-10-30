#include "Matrices.hpp"
#include "Constants.hpp"
Eigen::Matrix4f perspective(float fov, float aspect, float zNear, float zFar)
{
	float tanHalfFovy = tan(fov * 0.5f);

	Eigen::Matrix4f result = Eigen::Matrix4f::Zero();
	result(0, 0) = 1.0f / (aspect * tanHalfFovy);
	result(1, 1) = 1.0f / tanHalfFovy;
	result(2, 2) = (zFar + zNear) / (zNear - zFar);
	result(3, 2) = -1.0f;
	result(2, 3) = (2.0f * zFar * zNear) / (zNear - zFar);

	return result;
}

Eigen::Matrix4f makeTranslationMatrix(const Eigen::Vector3f& translate)
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();
	matrix.block<3, 1>(0, 3) = translate;
	return matrix;
}

Eigen::Matrix4f makeScaleMatrix(float scale)
{
	Eigen::Matrix4f scaleMat = Eigen::Matrix4f::Identity() * scale;
	scaleMat(3, 3) = 1.0f;
	return scaleMat;
}

Eigen::Matrix4f makeRotationMatrix(float x, float y, float z)
{

	Eigen::Matrix4f outMat = Eigen::Matrix4f::Identity();
	Eigen::Matrix3f rotMat = Eigen::Matrix3f(
		Eigen::AngleAxisf((x / 180) * M_PI, Eigen::Vector3f::UnitX()) *
		Eigen::AngleAxisf((y / 180) * M_PI, Eigen::Vector3f::UnitY()) *
		Eigen::AngleAxisf((z / 180) * M_PI, Eigen::Vector3f::UnitZ()));

	Eigen::Quaternion q = Eigen::AngleAxisf((x / 180) * M_PI, Eigen::Vector3f::UnitX()) *
		Eigen::AngleAxisf((y / 180) * M_PI, Eigen::Vector3f::UnitY()) *
		Eigen::AngleAxisf((z / 180) * M_PI, Eigen::Vector3f::UnitZ());
	
	outMat.block(0,0,3,3) = q.matrix();
	
	return outMat;
}

Eigen::Matrix4f makeIdentityMatrix(float scale)
{
	Eigen::Matrix4f identityMatrix = Eigen::Matrix4f::Identity();
	identityMatrix(0, 0) = scale;
	identityMatrix(1, 1) = scale;
	identityMatrix(2, 2) = scale;

	return identityMatrix;
}