#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;
}


void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}


void Lab4::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // TODO(student): Add transformation logic
    if (w) {
        translateZ -= 1 * deltaTime;
    }
    if (s) {
        translateZ += 1 * deltaTime;
    }
    if (a) {
        translateX -= 1 * deltaTime;
    }
    if (d) {
        translateX += 1 * deltaTime;
    }
    if (r) {
        translateY += 1 * deltaTime;
    }
    if (f) {
        translateY -= 1 * deltaTime;
    }

    if (p1) {
        scaleX -= 1 * deltaTime;
        scaleY -= 1 * deltaTime;
        scaleZ -= 1 * deltaTime;
    }
    if (p2) {
        scaleX += 1 * deltaTime;
        scaleY += 1 * deltaTime;
        scaleZ += 1 * deltaTime;
    }

    if (p3) {
        angularStepOX += glm::pi<float>() / 6 * deltaTime;

    }
    if (p4) {
        angularStepOX -= glm::pi<float>() / 6 * deltaTime;
    }

    if (p5) {
        angularStepOY += glm::pi<float>() / 6 * deltaTime;

    }
    if (p6) {
        angularStepOY -= glm::pi<float>() / 6 * deltaTime;
    }

    if (p7) {
        angularStepOZ += glm::pi<float>() / 6 * deltaTime;

    }
    if (p8) {
        angularStepOZ -= glm::pi<float>() / 6 * deltaTime;
    }
}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }

    if (key == GLFW_KEY_W) {
        w = true;
    }
    if (key == GLFW_KEY_S) {
        s = true;
    }
    if (key == GLFW_KEY_A) {
        a = true;
    }
    if (key == GLFW_KEY_D) {
        d = true;
    }
    if (key == GLFW_KEY_R) {
        r = true;
    }
    if (key == GLFW_KEY_F) {
        f = true;
    }

    if (key == GLFW_KEY_1) {
        p1 = true;
    }
    if (key == GLFW_KEY_2) {
        p2 = true;
    }

    if (key == GLFW_KEY_3) {
        p3 = true;
    }
    if (key == GLFW_KEY_4) {
        p4 = true;
    }

    if (key == GLFW_KEY_5) {
        p5 = true;
    }
    if (key == GLFW_KEY_6) {
        p6 = true;
    }

    if (key == GLFW_KEY_7) {
        p7 = true;
    }
    if (key == GLFW_KEY_8) {
        p8 = true;
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
    if (key == GLFW_KEY_W) {
        w = false;
    }
    if (key == GLFW_KEY_S) {
        s = false;
    }
    if (key == GLFW_KEY_A) {
        a = false;
    }
    if (key == GLFW_KEY_D) {
        d = false;
    }
    if (key == GLFW_KEY_R) {
        r = false;
    }
    if (key == GLFW_KEY_F) {
        f = false;
    }

    if (key == GLFW_KEY_1) {
        p1 = false;
    }
    if (key == GLFW_KEY_2) {
        p2 = false;
    }

    if (key == GLFW_KEY_3) {
        p3 = false;
    }
    if (key == GLFW_KEY_4) {
        p4 = false;
    }

    if (key == GLFW_KEY_5) {
        p5 = false;
    }
    if (key == GLFW_KEY_6) {
        p6 = false;
    }

    if (key == GLFW_KEY_7) {
        p7 = false;
    }
    if (key == GLFW_KEY_8) {
        p8 = false;
    }
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
