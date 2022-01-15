#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"

#include <string>
#include <vector>
#include <iostream>


namespace m2
{
    constexpr auto BOARD_SIZE = 8;
    constexpr auto CELL_SIZE = 1.0f;
    constexpr auto PIECE_SIZE = 0.4f;

    // Parameters related to piece surface generation
    constexpr auto no_of_generated_points = 10;           // number of points on a Bezier curve
    constexpr auto no_of_instances = 15;                   // number of instances (number of curves that contain the surface)
    constexpr auto max_translate = 0.0f;                  // for the translation surface, it's the distance between the first and the last curve
    constexpr auto max_rotate = glm::radians(360.0f);      // for the rotation surface, it's the angle between the first and the last curve

    constexpr glm::vec3 p11_piece_points[4] = {
        glm::vec3(0, 0, 1.0f),
        glm::vec3(0,  0.3f,  1.0f),
        glm::vec3(1.0f,  0.7f,  1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f)
    };

    constexpr glm::vec3 p12_piece_points[4] = {
        glm::vec3(0, 0, 1.0f),
        glm::vec3(0,  0.8f,  1.0f),
        glm::vec3(1.0f,  0.7f,  1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f)
    };

    constexpr glm::vec3 p13_piece_points[4] = {
        glm::vec3(0, 0, 1.0f),
        glm::vec3(0.4f,  0.3f,  1.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  0.0f)
    };

    enum class CellType {
        WHITE,
        BLACK
    };

    enum class PieceType {
        P1,
        P2
    };

    struct Cell {
        CellType type;
        int id;

        Cell() {
            type = CellType::WHITE;
            id = -1;
        }

        Cell(int id, CellType type): id(id), type(type){}
    };

    struct Piece {
        PieceType type;
        int id;
        int x;
        int y;

        Piece(int id, PieceType type, int x, int y): id(id), type(type), x(x), y(y){}
    };

    struct Board {
        Cell table[BOARD_SIZE][BOARD_SIZE];
        std::vector<Piece> pieces;


        Board() {

                int id = 0;
            // Generate Table 
            {
                bool isBlack = true;

                for (int i = 0; i < BOARD_SIZE; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j) {
                        table[i][j] = Cell(id, isBlack ? CellType::BLACK : CellType::WHITE);
 
                        id++;
                        isBlack = !isBlack;
                    }
                    isBlack = !isBlack;
                }
            }

            // Generate Pieces
            {
                // Player 1
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j) {
                        if (table[i][j].type == CellType::BLACK) {
                            pieces.push_back(Piece(id, PieceType::P1, i, j));
                            id++;
                        }
                    }
                }

                // Player 2
                for (int i = 5; i < BOARD_SIZE; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j) {
                        if (table[i][j].type == CellType::BLACK) {
                            pieces.push_back(Piece(id, PieceType::P2, i, j));
                            id++;
                        }
                    }
                }
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Board& b) {
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (b.table[i][j].type == CellType::BLACK) {
                        os << '#';
                    }
                    else {
                        os << '_';
                    }
                }
                os << '\n';
            }

            return os;
        }
    };

    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        unsigned int UploadCubeMapTexture(const std::string &pos_x, const std::string &pos_y, const std::string &pos_z, const std::string &neg_x, const std::string &neg_y, const std::string &neg_z);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void DrawBoard();
        void DrawPieces();
        void CustomColorRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void PieceRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::vec3 points[4]);

     private:
        int cubeMapTextureID;

        Board board;
    };
}   // namespace m2
