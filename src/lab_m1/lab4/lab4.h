#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Lab4 : public gfxc::SimpleScene
    {
     public:
        Lab4();
        ~Lab4();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        GLenum polygonMode;

        // Verfic daca tastele sunt apasate
        bool w = false, a = false, s = false, d = false, r = false, f = false;
        bool p1 = false, p2 = false;
        bool p3 = false, p4 = false, p5 = false, p6 = false, p7 = false, p8 = false;
    };
}   // namespace m1
