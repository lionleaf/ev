#include "physics_2d.h"
#include <iostream>
#include <math.h>

namespace phys {


    float SquaredDistance(Vec2f a, Vec2f b) {
        return pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0);
    }

    float Distance(Vec2f a, Vec2f b) {
        return sqrt(pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0));
    }

    float DotProduct(Vec2f a, Vec2f b) {
        return a.x * b.x + a.y * b.y;
    }

    bool AABBvsAABB(AABB a, AABB b) {
        if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
        if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
        return true;
    }
    bool CirclevsCircle(Circle a, Circle b) {
        float combined_r = a.radius + b.radius;
        combined_r *= combined_r;
        return combined_r > SquaredDistance(a.pos, b.pos);
    }

    Vec2f collisionNormal(Circle A, Circle B) {
        Vec2f unormalized = B.pos - A.pos;
        float len = sqrt(pow(unormalized.x, 2) + pow(unormalized.y, 2));
        return Vec2f{ unormalized.x / len, unormalized.y / len };
    }

    void ResolveCollision(PhysObject &A, PhysObject &B) {
        Vec2f relative_velocity = B.velocity - A.velocity;
        Vec2f normal = collisionNormal(A.circle, B.circle);

        float vel_along_normal = DotProduct(relative_velocity, normal);

        if (vel_along_normal > 0) {
            return; //Objects already moving apart
        }

        float restitution = fmin(A.restitution, B.restitution);

        float impulse_magnitude = -(1 + restitution) * vel_along_normal;
        impulse_magnitude /= 1.0 / A.mass + 1.0 / B.mass;

        Vec2f impulse = impulse_magnitude * normal;
        A.velocity =  A.velocity + (1.0 / A.mass) * impulse;
        B.velocity =  B.velocity - (1.0 / B.mass) * impulse;

    }

    void Simulator::add(PhysObject object)
    {
        m_objects.push_back(object);
    }

    void Simulator::step(float dt)
    {
        for (PhysObject &obj : m_objects) {
            obj.circle.pos = obj.circle.pos + obj.velocity * dt;
        }
        for (int i = 0; i < m_objects.size(); i++){
            PhysObject &obj_a = m_objects.at(i);
            for (int j = i + 1; j < m_objects.size(); j++){
                PhysObject &obj_b = m_objects.at(j);
                if (CirclevsCircle(obj_a.circle, obj_b.circle)) {
                    ResolveCollision(obj_a, obj_b);
                }
            }
        }

    }

    std::vector<PhysObject> Simulator::objects()
    {
        return m_objects;
    }


}