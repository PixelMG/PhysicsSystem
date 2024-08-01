#pragma once
#include "types.hpp"
#include "timestep.hpp"

namespace Engine
{
    class PhysicsBody
    {
        public:
        PhysicsBody(Rectangle position = { 0.0f, 0.0f, 0.0f, 0.0f }, Rectangle object = { 0.0f, 0.0f, 0.0f, 0.0f }, int objectID = -1);
        int GetObjectID() { return ObjectID; }
        void SetID(int id) { ID = id; }
        int GetID() { return ID; }
        void SetTag(std::string t) { tag = t; }
        bool CompareTag(std::string t) { return tag == t; }
        std::string GetTag();
        void SetObjectPosition(Vector2 position);
        void AddForce(float x, float y);
        void SetVelocity(float x, float y) { Velocity.x = x, Velocity.y = y; }
        void SetMaxVelocity(float maxVel) { maxVelocity = maxVel; }
        float GetX() { return Object.x; }
        float GetY() { return Object.y; }
        float ColX() { return Position.x; }
        float ColY() { return Position.y; }
        Vector2 GetVelocity() { return Velocity; }
        Vector2 GetCenter() { return Center; }
        float GetWidth() { return Width; }
        float GetHeight() { return Height; }
        void AlterBody(float w, float h);
        void SetOutofBoundsFlag(bool flag) { OutofBounds = flag; }
        void SetOverlappingFlag(bool flag) { Overlapping = flag; }
        // void SetOverlappingWith(int id) { OverlappingWith = id; }
        void PushOverlapping(int id) { OverlappingWith[olCount] = id; olCount++; }
        void FlushOverlapping();
        bool IsOutofBounds() { return OutofBounds; }
        bool IsOverlapping() { return Overlapping; }
        // int IsOverlappingWith() { return OverlappingWith; }
        int OverlapCount() { return olCount; }
        bool IsOverlappingWithTag(std::string t);
        void Disable() { isEnabled = false; }
        void Enable() { isEnabled = true; }
        bool IsEnabled() { return isEnabled; }
        bool IsGrounded() { return isGrounded; }
        virtual void Update(Timestep ts);
        virtual void HandleCollision(PhysicsBody* b2);

        protected:
        Rectangle Position;
        Rectangle Object;
        Vector2 Velocity;
        Vector2 Center;
        float Width;
        float Height;
        float maxVelocity = 1000;
        int ID;
        int ObjectID;
        std::string tag;
        bool OutofBounds = false;
        bool Overlapping = false;
        bool isGrounded = false;
        bool isEnabled = true;
        bool IsStatic = false;
        static const int OVERLAPMAX = 50;
        int OverlappingWith[OVERLAPMAX];
        int olCount = 0;
    };

    class StaticBody : public PhysicsBody
    {
        public:
        StaticBody(Rectangle position, Rectangle object, int objectID) :
            PhysicsBody(position, object, objectID) {}
        void HandleCollision(PhysicsBody* b2);

        private:
        bool IsStatic = true;
    };

    class Trigger : public PhysicsBody
    {
        public:
        Trigger(Rectangle position, Rectangle object, int objectID) :
            PhysicsBody(position, object, objectID) {}
        void HandleCollision(PhysicsBody* b2);
    };

    class RigidBody : public PhysicsBody
    {
        public:
        RigidBody(Rectangle position, Rectangle object, int objectID) :
            PhysicsBody(position, object, objectID) {}
        void Update(Timestep ts);
        // void HandleCollision(PhysicsBody* b2);
        void SetGravity(float g) { Gravity = g; }
        void SetDrag(Vector2 drag) { DragX = drag.x, DragY = drag.y; }

        private:
        float Gravity = 0.0f;
        float DragY = 0.5f;
        float DragX = 0.5f;
    };

    class Physics
    {
        public:
        Physics() {}
        void SetWorldBounds(float width, float height);
        void SetCameraOffsets(float x, float y) { camX = x; camY = y; }
        float GetWorldWidth() { return WorldWidth; }
        float GetWorldHeight() { return WorldHeight; }
        void AddBody(PhysicsBody* Body);
        void DestroyBody(int ID);
        PhysicsBody* GetBody(int ID) { return PhysicsBodies[ID]; }
        void Update(Timestep ts);
        void ClearBodies();
        int PhysicsBodyCount() { return b_count; }

        private:
        std::array<PhysicsBody*, 1000> PhysicsBodies;
        int b_count = 0;
        float WorldWidth;
        float WorldHeight;
        float camX = 0;
        float camY = 0;
        void CheckWorldBounds(PhysicsBody* b);
    };

    class PhysicsSystem
    {
        public:
        static void SetWorldBounds(float width, float height) { PhysicsEngine->SetWorldBounds(width, height); }
        static void SetCameraOffsets(float x, float y) { PhysicsEngine->SetCameraOffsets(x, y); }
        static float GetWorldWidth() { return PhysicsEngine->GetWorldWidth(); }
        static float GetWorldHeight() { return PhysicsEngine->GetWorldHeight(); }
        static void AddBody(PhysicsBody* Body) { PhysicsEngine->AddBody(Body); }
        static void DestroyBody(int ID) { PhysicsEngine->DestroyBody(ID); }
        static PhysicsBody* GetBody(int ID) { return PhysicsEngine->GetBody(ID); }
        static void Update(Timestep ts) { PhysicsEngine->Update(ts); }
        static void ClearBodies() { PhysicsEngine->ClearBodies(); }
        static int PhysicsBodyCount() { return PhysicsEngine->PhysicsBodyCount(); }

        private:
        PhysicsSystem() {}
        ~PhysicsSystem() {}
        static Physics* PhysicsEngine;
    };
};