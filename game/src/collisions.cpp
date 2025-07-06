#include "collisions.h"
#include "matrices.h"

bool Collisions::CheckAABBCollision(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool Collisions::CheckCowCollisionWithWorld(const AABB& cow, const std::vector<AABB>& obstacles) {
    for (const auto& obstacle : obstacles) {
        if (CheckAABBCollision(cow, obstacle)) {
            return true;
        }
    }
    return false;
}

bool Collisions::CheckPointSphereCollision(const glm::vec3& point, const glm::vec3& center, float radius) {
    glm::vec3 diff = point - center;
    float dist = dotproduct(glm::vec4(diff, 0.0f), glm::vec4(diff, 0.0f));
    return dist <= radius * radius;
}
