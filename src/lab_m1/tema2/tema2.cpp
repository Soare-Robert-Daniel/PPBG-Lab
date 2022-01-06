#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
    map = Map();
    camera = new tema2::Camera();
    isFirstPerson = false;


    int enemCounter = 0;
    for (int i = 0; i < map.rows; ++i) {
        for (int j = 0; j < map.cols; ++j) {
            if (map.data[i][j] == Cell::PATH && enemCounter < ENEMIES) {
                map.data[i][j] = Cell::ENEMY;
                enemies.push_back(Enemy(glm::vec3(i + 0.5f, 0.1f, j + 0.5f), glm::vec2(i, j)));
                enemCounter++;
            }
        }
    }
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;

    
    camera->Set(glm::vec3(2, 0.5f, 2), glm::vec3(2.5f, 0.1f, 2.5f), glm::vec3(0, 1, 0));

    player = new tema2::Player(camera->GetTargetPosition(), 2, "box");
    player->camera = camera;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader* shader = new Shader("HitEnemyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    
     projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, 0.01f, 200.0f);

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    
    currentCell.x = glm::floor(camera->GetTargetPosition().x);
    currentCell.y = glm::floor(camera->GetTargetPosition().z);

    // Move Enemies
    for (auto &e : enemies) {
        auto p = e.cell - currentCell;
        if (glm::abs(p.x) < 0.5f && glm::abs(p.y) < 0.5f) {
            // cout << "Cell " << currentCell.x << " " << currentCell.y << '\n';
            e.move(camera->GetTargetPosition(), deltaTimeSeconds);
        }
    }

    
    // Render Maze
    for (int i = 0; i < map.rows; ++i) {
        for (int j = 0; j < map.cols; ++j) {
            if (map.data[i][j] == Cell::WALL) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5f, 0.5f, j + 0.5f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(0.0f), glm::vec3(0, 1, 0));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
    }

    for (auto& e : enemies) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, e.position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(0.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["HitEnemyShader"], modelMatrix);
    }

    cout << "Projectile " << projectiles.size() << '\n';
    for (auto& p : projectiles) {
        p.move(deltaTimeSeconds);

        // TODO: delete the projectile when is over
        if (!p.isOver()) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, p.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
            RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
        }
    }


    if ( renderCameraTarget )
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        auto pos = camera->GetTargetPosition();
        pos.y = 0.1f;
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int timeLocation = glGetUniformLocation(shader->GetProgramID(), "time");
    glUniform1f(timeLocation, Engine::GetElapsedTime());

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{

        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->TranslateForward(deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->TranslateForward(-deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(deltaTime);
        }

        if (!isFirstPerson) {
            camera->position.y = 1;
        }
        else {
            camera->position.y = 0.2f;
        }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.

    if (window->KeyHold(GLFW_KEY_1)) {
        FOV += deltaTime * 10;

        left -= deltaTime * 10;
        right += deltaTime * 10;

        bottom -= deltaTime * 20;
        top += deltaTime * 20;
    }

    if (window->KeyHold(GLFW_KEY_2)) {
        FOV -= deltaTime * 10;

        left += deltaTime * 10;
        right -= deltaTime * 10;

        bottom += deltaTime * 20;
        top -= deltaTime * 20;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    if (key == GLFW_KEY_SPACE && isFirstPerson) {
        // Shoot projectile
        projectiles.push_back(Projectile(camera->GetTargetPosition(), camera->forward, PROJ_SPEED, PROJ_LIFETIME));
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (window->GetSpecialKeyState() == 0) {
        camera->RotateThirdPerson_OX(sensivityOX * deltaY * -1);
        camera->RotateThirdPerson_OY(sensivityOY * deltaX * -1);
    }

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        renderCameraTarget = false;
        isFirstPerson = true;

        camera->RotateFirstPerson_OX(sensivityOX * deltaY * -1);
        camera->RotateFirstPerson_OY(sensivityOY * deltaX * -1);

        
    }
    else {
        isFirstPerson = false;
        renderCameraTarget = true;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
