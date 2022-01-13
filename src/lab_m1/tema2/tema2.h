#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"
#include "lab_m1/tema2/player.h"
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <tuple>

constexpr auto MAP_SIZE = 20;
constexpr auto ENEMIES = 50;
constexpr auto PROJ_SPEED = 3.0f;
constexpr auto PROJ_LIFETIME = 1.0f;
constexpr auto MAX_GAMETIME = 5 * 60;

namespace m1
{
    enum class Cell { PLAYER, ENEMY, WALL, PATH};

    struct Map {

        int rows = MAP_SIZE;
        int cols = MAP_SIZE;
        Cell data[MAP_SIZE][MAP_SIZE];

        int dx[4] = { 1, -1, 0, 0 };
        int dy[4] = { 0, 0, -1, 1 };

        Map() {
            std::fill(*data, *data + rows * cols, Cell::WALL);

            // TODO: Replace with Gen Algh
            GenerateMap();
            
        }

        void GenerateMap() {
            int startX = rand() % MAP_SIZE + 1, startY = rand() % MAP_SIZE + 1;
            std::vector<std::tuple<int, int>> track = { std::make_tuple(startX, startY) };
            data[startX][startY] = Cell::PATH;

            int cx, cy, nx, ny;

            while (track.size()) {
                std::tie (cx, cy) = track.back();
                auto n = FindNeighbors(cx, cy);

                if (n.size() == 0) {
                    track.pop_back();
                }
                else {
                    std::tie(nx, ny) = n[rand() % n.size()];
                    data[nx][ny] = Cell::PATH;
                    data[(cx + nx) / 2][(cy + ny) / 2] = Cell::PATH;

                    track.push_back(std::make_tuple(nx, ny));
                }
            }

        }

        std::vector<std::tuple<int, int>> FindNeighbors(int x, int y) {
            std::vector<std::tuple<int, int>> n;

            if (x > 2 && data[x - 2][y] == Cell::WALL) {
                n.push_back(std::make_tuple(x - 2, y));
            }
            if (x < MAP_SIZE - 2 && data[x + 2][y] == Cell::WALL) {
                n.push_back(std::make_tuple(x + 2, y));
            }
            if (y > 2 && data[x][y-2] == Cell::WALL) {
                n.push_back(std::make_tuple(x, y - 2));
            }
            if (y < MAP_SIZE - 2 && data[x][y + 2] == Cell::WALL) {
                n.push_back(std::make_tuple(x, y + 2));
            }

            return n;
        }

        glm::vec3 GetStartPosition() {
            for (int i = 1; i < rows; ++i) {
                for (int j = 1; j < cols; ++j) {
                    if (data[rows][cols] == Cell::PATH) {
                        return glm::vec3(i + 0.5f, 0.1f, j + 0.5f);
                    }
                }
            }

            return glm::vec3(1.5f, 0.1f, 1.5f);
        }

       
    };

    struct Enemy {
        glm::vec3 position;
        glm::vec2 cell;
        float rotation;
        int life;
        float speed;
        float collRadius;
        float deathTime;
        bool isDying;

        Enemy(glm::vec3 p, glm::vec2 c, float coll) {
            position = p;
            cell = c;
            life = 5;
            rotation = 0.0f;
            speed = 0.5f;
            collRadius = coll;
            isDying = false;
            deathTime = 3.0f;

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

    struct Actor {
        float speed;
        std::string meshID;
        float life;
        float collRadius;

        Actor() {

        }

        Actor(std::string meshID, float speed, float life, float collRadius) {
            this->meshID = meshID;
            this->speed = speed;
            this->life = life;
            this->collRadius = collRadius;
        }

        void GetDamage() {
            life -= 1.0f;
        }

    };

    struct Projectile {
        glm::vec3 position;
        glm::vec3 direction;
        float speed;
        float lifetime;
        float collRadius;

        Projectile(glm::vec3 pos, glm::vec3 dir, float s, float l, float c) {
            position = pos;
            direction = dir;
            speed = s;
            lifetime = l;
            collRadius = c;
        }

        void move(float deltaTimeSeconds) {
            position += speed * direction * deltaTimeSeconds;
            lifetime -= deltaTimeSeconds;
        }

        bool isOver() {
            return lifetime < 0.0f;
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

        void DrawPlayer();
        void UI();
        // void RenderMesh2D(Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix);

     protected:
         tema2::Player *player;

         Map map;
         std::vector<Enemy> enemies;
         std::vector<Projectile> projectiles;

         Actor actor;

         glm::vec2 currentCell;
         glm::vec2 cameraCell;
         bool isFirstPerson;


        tema2::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float FOV = 60.f;

        float left = -1.f;
        float right = 1.f;
        float bottom = -1.f;
        float top = 1.f;
    };
}   // namespace m1
