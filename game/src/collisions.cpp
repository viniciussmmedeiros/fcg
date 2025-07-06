#include "collisions.h"
#include "matrices.h"

// Paredes fixas do jogo
std::vector<Collisions::AABB> Collisions::GetFixedObstacles() {
    return {
        {
            glm::vec3(-5.1f, -1.0f, -5.0f), glm::vec3(-4.9f, 2.0f, 5.0f) // esquerda
        }, 
        {
            glm::vec3(4.9f, -1.0f, -5.0f), glm::vec3(5.1f, 2.0f, 5.0f) // direita
        },
        {
            glm::vec3(-5.0f, -1.0f, -5.1f), glm::vec3(5.0f, 2.0f, -4.9f) // frontal
        },
        {
            glm::vec3(-5.0f, -1.0f, 4.9f), glm::vec3(5.0f, 2.0f, 5.1f) // trás
        }
    };
}

bool Collisions::CheckAABBCollision(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool Collisions::CheckPointSphereCollision(const glm::vec3& point, const glm::vec3& center, float radius) {
    glm::vec3 diff = point - center;
    
    float dist = dotproduct(glm::vec4(diff, 0.0f), glm::vec4(diff, 0.0f));
    
    return dist <= radius * radius;
}

Collisions::AABB Collisions::CreateCowAABB(const glm::vec3& position) {
    return {
        position + glm::vec3(-0.8f, 0.0f, -0.3f),
        position + glm::vec3( 0.8f, 1.2f,  0.3f)
    };
}

// altura: de 0 a size (1)
// size/2 nas direções x e z, considerando que 'position' é o centro e size é 1,
// criamos uma 'caixa' que engloba o nosso cubo
Collisions::AABB Collisions::CreateBoxAABB(const glm::vec3& position, float size) {
    return {
        position + glm::vec3(-size/2.0f, 0.0f, -size/2.0f),
        position + glm::vec3(size/2.0f, size, size/2.0f)
    };
}

// cria uma 'caixa' de apenas 0.04 unidades de altura, centralizada em 'position',
// para fazer o teste de colisão com o cubo
Collisions::AABB Collisions::CreateTileAABB(const glm::vec3& position, float size) {
    return {
        position + glm::vec3(-size, -0.02f, -size),
        position + glm::vec3(size, 0.02f, size)
    };
}

// verificamos apenas os eixos x e y, ignorando a altura, pois só
// queremos saber se horizontalmente a caixa está de alguma forma
// sobre esse pedaço do piso
bool Collisions::CheckBoxPlacement(const AABB& box, const AABB& tile) {
    bool x_overlap = (box.min.x <= tile.max.x && box.max.x >= tile.min.x);
    
    bool z_overlap = (box.min.z <= tile.max.z && box.max.z >= tile.min.z);
    
    return x_overlap && z_overlap;
}

bool Collisions::CanPushBox(const glm::vec3& newBoxPos, float boxSize, const std::vector<glm::vec3>& otherBoxPositions, size_t excludeIndex) {
    AABB newBox = CreateBoxAABB(newBoxPos, boxSize);
    
    // Verificar colisão com outras caixas
    for (size_t i = 0; i < otherBoxPositions.size(); ++i) {
        if (i != excludeIndex) {
            AABB otherBox = CreateBoxAABB(otherBoxPositions[i], boxSize);

            if (CheckAABBCollision(newBox, otherBox)) {
                return false;
            }
        }
    }
    
    // Verificar colisão com obstáculos fixos
    std::vector<AABB> fixedObstacles = GetFixedObstacles();
    for (const auto& obstacle : fixedObstacles) {
        if (CheckAABBCollision(newBox, obstacle)) {
            return false;
        }
    }
    
    return true;
}

bool Collisions::CheckCowMovementAndPushBoxes(const glm::vec3& newCowPos, const glm::vec3& movementDirection, std::vector<glm::vec3>& boxPositions, float boxSize) {
    AABB newCow = CreateCowAABB(newCowPos);

    // Verificar colisão com cada caixa
    for (size_t i = 0; i < boxPositions.size(); ++i) {
        AABB box = CreateBoxAABB(boxPositions[i], boxSize);

        if (CheckAABBCollision(newCow, box)) {
            // Tentar empurrar a caixa com velocidade proporcional ao movimento da vaca
            float pushDistance = 0.3f; // Distância base que a caixa será empurrada
            glm::vec3 pushDirection = glm::normalize(movementDirection);
            glm::vec3 newBoxPos = boxPositions[i] + pushDirection * pushDistance;

            if (CanPushBox(newBoxPos, boxSize, boxPositions, i)) {
                // Empurrar a caixa
                boxPositions[i] = newBoxPos;
                return true;
            } 
                
            return false;
        }
    }

    // Verificar colisão com obstáculos fixos (paredes)
    std::vector<AABB> fixedObstacles = GetFixedObstacles();
    for (const auto& obstacle : fixedObstacles) {
        if (CheckAABBCollision(newCow, obstacle)) {
            return false;
        }
    }

    return true;
}
