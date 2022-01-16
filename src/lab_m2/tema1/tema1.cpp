#include "lab_m2/tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "stb/stb_image.h"

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
    board = Board();
    showSelectionBuffer = false;
    selectedPiece = -1;
    std::cout << board;
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(-0.5, 8, 4), glm::quat(glm::vec3(-60 * TO_RADIANS, -90 * TO_RADIANS, 0)));
    camera->Update();

    // Create the selection buffer
    {
        auto resolution = window->GetResolution();
        selectionBuffer = new FrameBuffer();
        selectionBuffer->Generate(resolution.x, resolution.y, 1);
        FrameBuffer::BindDefault();
    }

    std::string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube");
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders");

    // Load Cube
    {
        Mesh* mesh = new Mesh("cube");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load Quad
    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for rendering to texture
    {
        Shader *shader = new Shader("CubeMap");
        shader->AddShader(PATH_JOIN(shaderPath, "CubeMap.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "CubeMap.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a shader program for rendering to texture
    {
        Shader *shader = new Shader("ShaderNormal");
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Cube Map Texture
    {
        cubeMapTextureID = UploadCubeMapTexture(
            PATH_JOIN(texturePath, "pos_x.png"),
            PATH_JOIN(texturePath, "pos_y.png"),
            PATH_JOIN(texturePath, "pos_z.png"),
            PATH_JOIN(texturePath, "neg_x.png"),
            PATH_JOIN(texturePath, "neg_y.png"),
            PATH_JOIN(texturePath, "neg_z.png"));
    }

    // Load Custom Color shader
    {
        Shader* shader = new Shader("CustomColor");
        shader->AddShader(PATH_JOIN(shaderPath, "CustomColor.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "CustomColor.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a shader program for piece surface generation
    {
        Shader* shader = new Shader("PieceSurface");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders", "Piece.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders", "Piece.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders", "Piece.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a bogus mesh with 2 points (a line)
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(p11_piece_points[0], glm::vec3(0, 1, 1)),
            VertexFormat(p11_piece_points[3], glm::vec3(0, 1, 0))
        };

        vector<unsigned int> indices =
        {
            0, 1
        };

        meshes["surface"] = new Mesh("generated initial surface points");
        meshes["surface"]->InitFromData(vertices, indices);
        meshes["surface"]->SetDrawMode(GL_LINES);
    }
}


void Tema1::FrameStart()
{
}


void Tema1::Update(float deltaTimeSeconds)
{
    ClearScreen();

    auto camera = GetSceneCamera();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        selectionBuffer->Bind();
        useIdAsAlpha = true;
        Draw();
        useIdAsAlpha = false;

        // x si y reprezinta pozitia mouse-ului pe ecran, in pixeli.
        unsigned char data[4];
        auto p = window->GetCursorPosition(); auto x = p.x; auto y = p.y;
        y = window->props.resolution.y - y;

        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        pieceId = data[3];
        // std::cout << pieceId << " ";

    }

    if (!showSelectionBuffer)
    {
        selectionBuffer->BindDefault();
        // Draw the cubemap
        {
            Shader *shader = shaders["ShaderNormal"];
            shader->Use();

            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));

            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
            int loc_texture = shader->GetUniformLocation("texture_cubemap");
            glUniform1i(loc_texture, 0);

            meshes["cube"]->Render();
        }

        // Draw the reflection on the mesh
        {
            Shader *shader = shaders["CubeMap"];
            shader->Use();

            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.1f));

            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

            auto cameraPosition = camera->m_transform->GetWorldPosition();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
            int loc_texture = shader->GetUniformLocation("texture_cubemap");
            glUniform1i(loc_texture, 0);

            int loc_camera = shader->GetUniformLocation("camera_position");
            glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);
        }
 
        Draw();
    }
}


void Tema1::FrameEnd()
{
    DrawCoordinateSystem();
}


unsigned int Tema1::UploadCubeMapTexture(const std::string &pos_x, const std::string &pos_y, const std::string &pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z)
{
    int width, height, chn;

    unsigned char* data_pos_x = stbi_load(pos_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_y = stbi_load(pos_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_z = stbi_load(pos_z.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_x = stbi_load(neg_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_y = stbi_load(neg_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_z = stbi_load(neg_z.c_str(), &width, &height, &chn, 0);

    unsigned int textureID = 0;
    // TODO(student): Create the texture
    glGenTextures(1, &textureID);

    // TODO(student): Bind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxAnisotropy;

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // TODO(student): Load texture information for each face
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_z);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_z);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
    }

    // Free memory
    SAFE_FREE(data_pos_x);
    SAFE_FREE(data_pos_y);
    SAFE_FREE(data_pos_z);
    SAFE_FREE(data_neg_x);
    SAFE_FREE(data_neg_y);
    SAFE_FREE(data_neg_z);

    return textureID;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    std::cout << button << ' ';
    if (button == GLFW_MOUSE_BUTTON_2) {

        if (pieceId != selectedPiece) {
            selectedPiece = pieceId;
        }
        else {
            selectedPiece = -1;
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}

void m2::Tema1::DrawBoard()
{
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i * CELL_SIZE + 0.5f, 0.0f, j * CELL_SIZE + 0.5f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(CELL_SIZE * 0.5f));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
            if (board.table[i][j].type == CellType::BLACK) {
                CustomColorRenderMesh(meshes["quad"], shaders["CustomColor"], modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f), board.table[i][j].id);
            }
            else {
                CustomColorRenderMesh(meshes["quad"], shaders["CustomColor"], modelMatrix, glm::vec3(0.8f, 0.8f, 0.8f), board.table[i][j].id);
            }
        }
    }
}

void m2::Tema1::CustomColorRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const int id)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform1i(glGetUniformLocation(shader->program, "object_id"), useIdAsAlpha ? id : 255);

    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

    mesh->Render();
}

void m2::Tema1::DrawPieces()
{
    for (auto& p : board.pieces) {

        // Part 1
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(p.x + 0.5f, 0.0f, p.y + 0.5f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PIECE_SIZE, 1.0f, PIECE_SIZE));
            auto color = p.type == PieceType::P1 ? glm::vec3(0.0f, 0.0f, 0.8f) : glm::vec3(0.8f, 0.0f, 0.8f);
            color = p.id == selectedPiece ? glm::vec3(0.0f, 1.0f, 0.0f) : color;
            auto points = p.type == PieceType::P1 ? p11_piece_points : p21_piece_points;
            PieceRenderMesh(meshes["surface"], shaders["PieceSurface"], modelMatrix, color, points, p.id);
        }
        // Part 2
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(p.x + 0.5f, 1.0f * PIECE_SIZE, p.y + 0.5f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PIECE_SIZE, 1.0f, PIECE_SIZE));
            auto color = p.type == PieceType::P1 ? glm::vec3(0.0f, 0.0f, 0.8f) : glm::vec3(0.8f, 0.0f, 0.8f);
            color = p.id == selectedPiece ? glm::vec3(0.0f, 1.0f, 0.0f) : color;
            auto points = p.type == PieceType::P1 ? p12_piece_points : p22_piece_points;
            PieceRenderMesh(meshes["surface"], shaders["PieceSurface"], modelMatrix, color, points, p.id);
        }
        // Part 3
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(p.x + 0.5f, 2.0f * PIECE_SIZE, p.y + 0.5f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PIECE_SIZE, 1.0f, PIECE_SIZE));
            auto color = p.type == PieceType::P1 ? glm::vec3(0.0f, 0.0f, 0.8f) : glm::vec3(0.8f, 0.0f, 0.8f);
            color = p.id == selectedPiece ? glm::vec3(0.0f, 1.0f, 0.0f) : color;
            auto points = p.type == PieceType::P1 ? p13_piece_points : p23_piece_points;
            PieceRenderMesh(meshes["surface"], shaders["PieceSurface"], modelMatrix, color, points, p.id);
        }
    }
}

void m2::Tema1::Draw()
{
    DrawBoard();
    DrawPieces();
}

void m2::Tema1::PieceRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::vec3 points[4], const int id)
{
    if (!mesh || !shader || !shader->program)
        return;


    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Render an object using the specified shader and the specified position
    shader->Use();

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Surface Parameters
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

    // Bezier Points
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), points[0].x, points[0].y, points[0].z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), points[1].x, points[1].y, points[1].z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), points[2].x, points[2].y, points[2].z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), points[3].x, points[3].y, points[3].z);

    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shader->program, "object_id"), useIdAsAlpha ? id : 255);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, no_of_instances);
}
