#pragma once

#include <glm/vec3.hpp>
#include <vector>

class Collisions {
public:
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    static bool CheckAABBCollision(const AABB& a, const AABB& b);
    static bool CheckCowCollisionWithWorld(const AABB& cow, const std::vector<AABB>& obstacles);
    static bool CheckPointSphereCollision(const glm::vec3& point, const glm::vec3& sphereCenter, float radius);
};