#include "physics.hpp"

using namespace Engine;

PhysicsBody::PhysicsBody(Rectangle position, Rectangle object, int objectID)
{
    Position = position, Width = object.w, Height = object.h, ObjectID = objectID;
    Object = object;
    Center = { Object.x + Width / 2, Object.y + Height / 2 };
    tag = "";
}

std::string PhysicsBody::GetTag() { return tag; }

void PhysicsBody::AddForce(float x, float y)
{
    Velocity.x += x;
    // Velocity.y += Velocity.y + y > maxVelocity || Velocity.y + y < -maxVelocity ? 0 : y;
    Velocity.y += y;
}

void PhysicsBody::SetObjectPosition(Vector2 position)
{
    Object.x = position.x, Object.y = position.y;
    Center = { Object.x + Width / 2, Object.y + Height / 2 };
}

void PhysicsBody::AlterBody(float w, float h)
{
    float diff = Height - h;
    Width = w, Height = h;
}

bool PhysicsBody::IsOverlappingWithTag(std::string t)
{
    for(int i = 0; i < olCount; i++)
    {
        if(PhysicsSystem::GetBody(OverlappingWith[i])->GetTag() == t) return true;
    }
    return false;
}

void PhysicsBody::FlushOverlapping()
{
    for(int i = 0; i < olCount; i++)
    {
        OverlappingWith[i] = -1;
    }
    olCount = 0;
}

void PhysicsBody::Update(Timestep ts)
{
    SetObjectPosition({ Object.x + Velocity.x, Object.y + Velocity.y });
    Velocity = { 0.0f, 0.0f };
}

void PhysicsBody::HandleCollision(PhysicsBody* b2) {}

void StaticBody::HandleCollision(PhysicsBody* b2)
{
    if(b2->GetCenter().y < GetCenter().y && b2->GetY() < GetY() &&
        b2->GetX() + b2->GetWidth() - b2->GetVelocity().x > GetX() &&
        b2->GetX() - b2->GetVelocity().x < GetX() + GetWidth() &&
        b2->GetVelocity().y > 0)
    {
        float diff = GetY() - b2->GetY() - b2->GetHeight();
        b2->SetVelocity(b2->GetVelocity().x, 0.0f);
        b2->SetObjectPosition({ b2->GetX() + GetVelocity().x, b2->GetY() + diff });
    }

    if(b2->GetCenter().y > GetCenter().y && b2->GetY() > GetY() &&
        b2->GetX() + b2->GetWidth() - b2->GetVelocity().x > GetX() &&
        b2->GetX() - b2->GetVelocity().x < GetX() + GetWidth() &&
        b2->GetVelocity().y < 0)
    {
        float diff = GetY() + GetHeight() - b2->GetY();
        b2->SetVelocity(b2->GetVelocity().x, 0.0f);
        b2->SetObjectPosition({ b2->GetX(), b2->GetY() + diff });
    }

    // left side
    if(b2->GetCenter().x < GetCenter().x && b2->GetX() < GetX() &&
        b2->GetY() + b2->GetHeight() - b2->GetVelocity().y > GetY() &&
        b2->GetY() - b2->GetVelocity().y < GetY() + GetHeight() &&
        b2->GetVelocity().x > 0)
    {
        float diff = b2->GetX() + b2->GetWidth() - GetX();
        b2->SetVelocity(0.0f, b2->GetVelocity().y);
        b2->SetObjectPosition({ b2->GetX() - diff, b2->GetY() });
    }

    if(b2->GetCenter().x > GetCenter().x && b2->GetX() + b2->GetWidth() > GetX() + GetWidth() &&
        b2->GetY() + b2->GetHeight() - b2->GetVelocity().y > GetY() &&
        b2->GetY() - b2->GetVelocity().y < GetY() + GetHeight() &&
        b2->GetVelocity().x < 0)
    {
        float diff = GetX() + GetWidth() - b2->GetX();
        b2->SetVelocity(0.0f, b2->GetVelocity().y);
        b2->SetObjectPosition({ b2->GetX() + diff, b2->GetY() });
    }

}

void Trigger::HandleCollision(PhysicsBody* b2) {}

void RigidBody::Update(Timestep ts)
{
    if(Velocity.x > 0 && Velocity.x > maxVelocity * ts.GetSeconds()) Velocity.x = maxVelocity * ts.GetSeconds();
    if(Velocity.x < 0 && Velocity.x < -maxVelocity * ts.GetSeconds()) Velocity.x = -maxVelocity * ts.GetSeconds();
    if(Velocity.x > 0) Velocity.x += -(DragX * ts.GetSeconds());
    if(Velocity.x < 0) Velocity.x += (DragX * ts.GetSeconds());
    if(Velocity.x < 0.025f && Velocity.x > -0.025f) Velocity.x = 0;

    auto x = Object.x - Position.x;

    SetObjectPosition({ Object.x + Velocity.x, Object.y + Velocity.y});
}

void Physics::SetWorldBounds(float width, float height)
{
    WorldWidth = width, WorldHeight = height;
}

void Physics::CheckWorldBounds(PhysicsBody* b)
{
    if(b->GetX() + b->GetWidth() > WorldWidth ||
        b->GetX() < 0 ||
        b->GetY() + b->GetHeight() > WorldHeight ||
        b->GetY() < 0)
    {
       b->SetOutofBoundsFlag(true);
       return;
    }

    b->SetOutofBoundsFlag(false);
}

void Physics::AddBody(PhysicsBody* Body)
{
    Body->SetID(b_count);
    PhysicsBodies[b_count] = Body;
    b_count++;
}

void Physics::DestroyBody(int ID)
{
    delete PhysicsBodies[ID];
    PhysicsBodies[ID] = nullptr;

    for(int i = ID + 1; i < b_count; i++)
    {
        PhysicsBodies[i - 1] = PhysicsBodies[i];
        PhysicsBodies[i - 1]->SetID(i - 1);
    }

    b_count--;
    return;
}

int physTime = 0;

void Physics::Update(Timestep ts)
{
    PhysicsBody* bodies[2][500];
    int pos = 0;
    int qdc[2] = { 0, 0 };
    int qd = 2;

    // clear collisions
    for(int i = 0; i < b_count; i++)
    {
        auto b = PhysicsBodies[i];
        b->Update(ts);
        b->SetOverlappingFlag(false);
        b->FlushOverlapping();
        
        if(b->GetX() < (WorldWidth + 16 + (camX * -1) / 2) &&
            b->GetX() + b->GetWidth() > (camX * -1))
        {
            int q = qdc[0];
            bodies[0][q] = b;
            qdc[0]++;
            pos++;
        }

        if(b->GetX() < WorldWidth + (camX * -1) &&
            b->GetX() + b->GetWidth() > (camX * -1) + (WorldWidth / 2) - 16)
        {
            int q = qdc[1];
            bodies[1][q] = b;
            qdc[1]++;
            pos++;
        }
    }

    int checks = 0;

    // check collisions
    for(int q = 0; q < qd; q++)
    {
        int qq = qdc[q];
        for(int a = 0; a < qq; a++)
        {
            auto b1 = bodies[q][a];

            CheckWorldBounds(b1);

            for(int b = 0; b < qq; b++)
            {
                auto b2 = bodies[q][b];

                if(b1->GetID() != b2->GetID() &&
                    b1->IsEnabled() && b2->IsEnabled())
                {
                    if(b1->GetX() + b1->GetWidth() > b2->GetX() &&
                        b1->GetX() < b2->GetX() + b2->GetWidth() &&
                        b1->GetY() + b1->GetHeight() > b2->GetY() &&
                        b1->GetY() < b2->GetY() + b2->GetHeight())
                    {
                        b1->SetOverlappingFlag(true);
                        b1->PushOverlapping(b2->GetID());
                        b2->SetOverlappingFlag(true);
                        b2->PushOverlapping(b1->GetID());

                        // b1->AddForce(b2->GetVelocity().x, 0);

                        b1->HandleCollision(b2);
                    }
                }
                checks++;
            }
        }
    }

    for(int q = 0; q < qd; q++)
    {
        for(int i = 0; i < pos; i++)
        {
            bodies[q][i] = nullptr;
        }
    }
}

void Physics::ClearBodies()
{
    for(int i = 0; i < b_count; i++)
    {
        delete PhysicsBodies[i];
        PhysicsBodies[i] = nullptr;
    }

    b_count = 0;
}

Physics* PhysicsSystem::PhysicsEngine = new Physics();