#pragma once

#include <glm/vec3.hpp>
#include <vector>

class Collisions {
public:
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    static std::vector<AABB> GetFixedObstacles();

    static bool CheckAABBCollision(const AABB& a, const AABB& b);
    static bool CheckPointSphereCollision(const glm::vec3& point, const glm::vec3& sphereCenter, float radius);
    static bool CheckBoxPlacement(const AABB& box, const AABB& tile);
    static bool CanPushBox(const glm::vec3& newBoxPos, float boxSize, const std::vector<glm::vec3>& otherBoxPositions, size_t excludeIndex);
    static bool CheckCowMovementAndPushBoxes(const glm::vec3& newCowPos, const glm::vec3& movementDirection, std::vector<glm::vec3>& boxPositions, float boxSize);
    
    static AABB CreateCowAABB(const glm::vec3& position);
    static AABB CreateBoxAABB(const glm::vec3& position, float size);
    static AABB CreateTileAABB(const glm::vec3& position, float size);
};