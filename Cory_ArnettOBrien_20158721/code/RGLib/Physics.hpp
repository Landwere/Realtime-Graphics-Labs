#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <memory>
#include <SDL_stdinc.h>



namespace RGLib 
{

	class Physics
	{
	public:
		Physics();
		~Physics();
		void StepSimulation(Uint64 stepTime);
		void CreatePysicsBox(float width, float height, bool _static = true);
		void CreatePhysicsSphere(float diameter, bool _static = false);
		btDiscreteDynamicsWorld GetWorld();
	private:
		std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig =
			std::make_unique<btDefaultCollisionConfiguration>();
		std::unique_ptr<btCollisionDispatcher> dispatcher =
			std::make_unique<btCollisionDispatcher>(collisionConfig.get());
		std::unique_ptr<btBroadphaseInterface> overlappingPairCache =
			std::make_unique<btDbvtBroadphase>();
		std::unique_ptr<btSequentialImpulseConstraintSolver> solver =
			std::make_unique<btSequentialImpulseConstraintSolver>();
		std::unique_ptr<btDiscreteDynamicsWorld> world =
			std::make_unique<btDiscreteDynamicsWorld>(
				dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfig.get());
	};

	class PSphere
	{
	public:
		PSphere(float diameter);

		void SetTransform(float x, float y, float z);
		void GetTransform(btTransform tran);
		void ApplyForce(float x, float y, float z);
		void SetDynamic();
		void Activate();
		btRigidBody GetRB();

		btRigidBody* ball;

	private:

	};
}