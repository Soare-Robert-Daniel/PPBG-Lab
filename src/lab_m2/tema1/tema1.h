#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/frame_buffer.h"
#include "core/gpu/particle_effect.h"

#include <string>
#include <vector>
#include <iostream>
#include <tuple>


namespace m2
{
	constexpr auto BOARD_SIZE = 8;
	constexpr auto CELL_SIZE = 1.0f;
	constexpr auto PIECE_SIZE = 0.4f;
	constexpr auto PIECE_SPEED = 2.0f;

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
		glm::vec3(0, 0.71f, 1.0f),
		glm::vec3(0.5f,  0.8f,  1.0f),
		glm::vec3(0.9f,  0.7f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f)
	};

	constexpr glm::vec3 p21_piece_points[4] = {
		glm::vec3(0, 0, 1.0f),
		glm::vec3(0,  0.3f,  1.0f),
		glm::vec3(1.0f,  0.7f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f)
	};

	constexpr glm::vec3 p22_piece_points[4] = {
		glm::vec3(0, 0, 1.0f),
		glm::vec3(0,  0.8f,  1.0f),
		glm::vec3(1.0f,  0.7f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f)
	};

	constexpr glm::vec3 p23_piece_points[4] = {
		glm::vec3(0, 0.71f, 1.0f),
		glm::vec3(0.5f,  0.8f,  1.0f),
		glm::vec3(0.9f,  0.7f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f)
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

		Cell(int id, CellType type) : id(id), type(type) {}
	};

	struct Piece {
		PieceType type;
		int id;
		int x;
		int y;

		float cX;
		float cY;
		float tX;
		float tY;

		Piece(int id, PieceType type, int x, int y) : id(id), type(type), x(x), y(y) {
			cX = tX = (float)x;
			cY = tY = (float)y;
		}

		void move(float deltaTime) {
			if ( glm::abs( tX - cX ) > 0.01f) {
				auto sign = tX > cX ? 1.0f : -1.0f;
				cX += sign * glm::min(deltaTime * PIECE_SPEED, glm::abs(tX - cX));;
			}
			if (glm::abs(tY - cY) > 0.01f) {
				auto sign = tY > cY ? 1.0f : -1.0f;
				cY += sign * glm::min( deltaTime * PIECE_SPEED, glm::abs(tY - cY));
			}
		}
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

		bool HasPiece(int x, int y) {
			for (auto& p : pieces) {
				if (p.x == x && p.y == y) {
					return true;
				}
			}
			return false;
		}

		bool PieceExists(int id) {
			for (auto& p : pieces) {
				if (p.id == id) {
					return true;
				}
			}
			return false;
		}

		Piece GetPiece(int x, int y) {
			for (auto& p : pieces) {
				if (p.x == x && p.y == y) {
					return p;
				}
			}
		}

		Piece GetPieceById(int id) {
			for (auto& p : pieces) {
				if (p.id == id) {
					return p;
				}
			}

			return Piece(-1, PieceType::P1, 1, 1);
		}

		void RemovePiece(int id) {
			if (id < -1) {
				return;
			}
			for (int i = 0; i < pieces.size(); ++i) {
				if (pieces[i].id == id) {
					pieces.erase(pieces.begin() + i);
					break;
				}
			}
		}


		void MakeTakeOver(Piece& source, int destId, int targetId ) {

			RemovePiece(targetId);

			for (int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					if (table[i][j].id == destId) {
						
						for (auto& p : pieces) {
							if (p.id == source.id) {
								p.x = i;
								p.y = j;
								p.tX = (float)i;
								p.tY = (float)j;
								break;
							}
						}
						return;
					}
				}
			}
		}

		std::vector<Cell> GetFreeMovement(Piece p) {
			std::vector<Cell> cells;
			int left = p.y - 1;
			int right = p.y + 1;

			if (p.type == PieceType::P1) {
				int forward = p.x + 1;
				if (0 <= left && !HasPiece(forward, left)) {
					cells.push_back(table[forward][left]);
				}
				if (right < BOARD_SIZE && !HasPiece(forward, right)) {
					cells.push_back(table[forward][right]);
				}
			}
			else {
				int forward = p.x - 1;
				if (0 <= left && !HasPiece(forward, left)) {
					cells.push_back(table[forward][left]);
				}
				if (right < BOARD_SIZE && !HasPiece(forward, right)) {
					cells.push_back(table[forward][right]);
				}
			}

			return cells;
		}

		std::vector<std::tuple<Cell, Piece>> GetTakeOverMovement(Piece p) {
			std::vector<std::tuple<Cell, Piece>> cells;

			int left = p.y - 2;
			int right = p.y + 2;

			// TODO: verify out of bound for forward
			if (p.type == PieceType::P1) {
				int forward = p.x + 2;
				if (0 <= left && !HasPiece(forward, left) && HasPiece(p.x + 1, p.y - 1)) {
					auto t = GetPiece(p.x + 1, p.y - 1);
					if (t.type == PieceType::P2) {
						cells.push_back(std::make_tuple(table[forward][left], t));
					}
				}
				if (right < BOARD_SIZE && !HasPiece(forward, right)) {
					auto t = GetPiece(p.x + 1, p.y + 1);
					if (t.type == PieceType::P2) {
						cells.push_back(std::make_tuple(table[forward][right], t));
					}
				}
			}
			else {
				int forward = p.x - 2;
				if (0 <= left && !HasPiece(forward, left) && HasPiece(p.x - 1, p.y - 1)) {
					auto t = GetPiece(p.x - 1, p.y - 1);
					if (t.type == PieceType::P1) {
						cells.push_back(std::make_tuple(table[forward][left], GetPiece(p.x - 1, p.y - 1)));
					}

				}
				if (right < BOARD_SIZE && !HasPiece(forward, right)) {
					auto t = GetPiece(p.x - 1, p.y + 1);
					if (t.type == PieceType::P1) {
						cells.push_back(std::make_tuple(table[forward][right], GetPiece(p.x - 1, p.y + 1)));
					}
				}
			}

			return cells;
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

	struct Particle
	{
		glm::vec4 position;
		glm::vec4 speed;
		glm::vec4 initialPos;
		glm::vec4 initialSpeed;

		Particle() {}

		Particle(const glm::vec4& pos, const glm::vec4& speed)
		{
			SetInitial(pos, speed);
		}

		void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
		{
			position = pos;
			initialPos = pos;

			this->speed = speed;
			initialSpeed = speed;
		}
	};

	struct ParticleModel {
		ParticleEffect<Particle>* particleEffect;
		float lifetime;

		ParticleModel(float x, float y, float z) {
			lifetime = 2.0f;
			unsigned int nrParticles = 15000;

			particleEffect = new ParticleEffect<Particle>();
			particleEffect->Generate(nrParticles, true);

			auto particleSSBO = particleEffect->GetParticleBuffer();
			Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

			for (unsigned int i = 0; i < nrParticles; i++)
			{
				glm::vec4 pos(1);
				pos.x = x;
				pos.y = y;
				pos.z = z;

				glm::vec4 speed(0);
				speed.x = (rand() % 50) / 50.0f;
				speed.z = (rand() % 50) / 50.0f;
				speed.y = rand() % 2 + 1.0f;

				data[i].SetInitial(pos, speed);
			}

			particleSSBO->SetBufferData(data);
		}

		void update(float deltaTime) {
			lifetime -= deltaTime;
		}

		bool hasFinished() {
			return lifetime < 0.0f;
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

		unsigned int UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void Draw();
		void DrawBoard();
		void DrawPieces();
		void CustomColorRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const int id);
		void PieceRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::vec3 points[4], const int id);
		void LoadShader(const std::string& name, bool hasGeomtery = true);

	private:
		int cubeMapTextureID;

		Board board;
		FrameBuffer* selectionBuffer;
		GLenum polygonMode;
		bool showSelectionBuffer;
		bool useIdAsAlpha;
		int selectedPiece;
		int pieceId;
		std::vector<int> highlight;
		std::vector<Cell> free;
		std::vector<std::tuple<Cell, Piece>> takeover;
		std::vector<ParticleModel> particleModels;
	};
}   // namespace m2
