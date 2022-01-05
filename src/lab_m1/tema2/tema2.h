#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"
#include "lab_m1/tema2/player.h"
#include <vector>

constexpr auto MAP_SIZE = 20;
constexpr auto ENEMIES = 50;

namespace m1
{
    enum class Cell { PLAYER, ENEMY, WALL, PATH};

    struct Map {
        Map() {
            std::fill(*data, *data + rows * cols, Cell::WALL);

            // TODO: Replace with Gen Algh
            for (int i = 1; i < rows - 1; ++i) {
                for (int j = 1; j < cols - 1; ++j) {
                    if (j % 2 == 0 || i % 2 == 0) {
                        data[i][j] = Cell::PATH;
                    }
                }
            }
        }

        int rows = MAP_SIZE;
        int cols = MAP_SIZE;
        Cell data[MAP_SIZE][MAP_SIZE];
    };

    struct Enemy {
        glm::vec3 position;
        glm::vec2 cell;
        float rotation;
        int life;
        float speed;

        Enemy(glm::vec3 p, glm::vec2 c) {
            position = p;
            cell = c;
            life = 5;
            rotation = 0.0f;
            speed = 1.0f;
        }

        void move(glm::vec3 target, float deltaTimeSeconds) {
            // TODO: stop the movement if the difference is small
            auto dir = glm::normalize(target - position);
            dir.y = 0;
            position += speed * dir * deltaTimeSeconds;

            position.x = glm::clamp(position.x, cell.x, cell.x + 1);
            position.z = glm::clamp(position.z, cell.y, cell.y + 1);
        }

    };

    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
         tema2::Player *player;

         Map map;
         std::vector<Enemy> enemies;

         glm::vec2 currentCell;


        tema2::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        int projectionType = 0;
        float FOV = 60.f;

        float left = -3.f;
        float right = 3.f;
        float bottom = -3.f;
        float top = 3.f;
    };
}   // namespace m1
