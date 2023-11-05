#pragma once

#include <Eigen/Dense>
#include <cmath>
//Source David Walton (Moodle.bcu.ac.uk)

Eigen::Matrix4f perspective(float fov, float aspect, float zNear, float zFar);

Eigen::Matrix4f makeTranslationMatrix(const Eigen::Vector3f& translate);

Eigen::Matrix4f makeScaleMatrix(float scale);

//end source