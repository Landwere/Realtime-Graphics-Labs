#include "Physics.hpp"

namespace RGLib
{
	Physics::Physics()
	{
		world->setGravity(btVector3(0, -1, 0));

	}

	void Physics::StepSimulation(Uint64 stepTime)
	{
		world->stepSimulation((stepTime / 1000.0f), 10);
	}

	void Physics::CreatePysicsBox(float width, float height, bool _static) /* Static = true by default*/
	{
		btBoxShape* box;
		box = new btBoxShape(btVector3(width, height, width));
		btRigidBody* floor;
		int s = 0;
		if (!_static) /*if static is set to false set s to 1, 1 is dynamic*/
			s = 1;
		btRigidBody::btRigidBodyConstructionInfo floorInfo{ 0, 0, box };
		floor = new btRigidBody(floorInfo);
		floor->setRestitution(0.4f);
		floor->setCollisionShape(box);
		floor->setWorldTransform(btTransform(btQuaternion(0, 0, 0), btVector3(0.0f, 0.f, 0.0f)));
		world->addCollisionObject(floor);
	}

	btDiscreteDynamicsWorld Physics::GetWorld()
	{
		return *world;
	}


	PSphere::PSphere(float diameter)
	{
		btSphereShape* sphere;
		sphere = new btSphereShape(btScalar(diameter));
		btTransform ballTr;
		ballTr.setOrigin(btVector3(0, 0, 0));
		//sphere->calculateLocalInertia(1, btVector3(0, 0, 0));
		btDefaultMotionState* ballMS = new btDefaultMotionState(ballTr);

		btRigidBody::btRigidBodyConstructionInfo ballInfo{ 0, ballMS, sphere };
		ball = new btRigidBody(ballInfo);
		ball->setRestitution(0.6f);
		ball->setCollisionShape(sphere);
		//ball->setWorldTransform(btTransform(btQuaternion(0, 0, 0), btVector3(4.5f, 2.6f, 4.0f)));
		ball->setActivationState(DISABLE_DEACTIVATION);
	}
	void PSphere::SetTransform(float x, float y, float z)
	{
		ball->setWorldTransform(btTransform(btQuaternion(0, 0, 0), btVector3(x, y, z)));
	}
	void PSphere::GetTransform(btTransform tran)
	{
		ball->getMotionState()->getWorldTransform(tran);
	}
	void PSphere::ApplyForce(float x, float y, float z)
	{

	}
	void PSphere::SetDynamic()
	{
		ball->setMassProps(1, btVector3(0.0f, 1.f, 0)); /*setting mass to 1 allows the object to move*/
	}
	void PSphere::Activate()
	{
		ball->activate();
	}
	btRigidBody PSphere::GetRB()
	{
		return *ball;
	}
}

