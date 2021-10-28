#pragma once

#include "components/simple_scene.h"

namespace m1
{
    class Lab1 : public gfxc::SimpleScene
    {
     public:
        Lab1();
        ~Lab1();

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

        // TODO(student): Class variables go here
        int clearColorOption = 0; // Indicele culoarii pe dorim sa o folosim la stergere
        static const int nrClearColorOptions = 2; // Numarul de culori de stergere
        float clearColorOptions[nrClearColorOptions][4] = { {0, 0, 0, 1.0f}, {0.5f, 0.2f, 0, 1.0f} }; // Vectori de culori care pot fi folosit la stergere

        int objectToRenderOption = 0; // Indicele obiectului pe care dorim sa-l afisam
        std::vector<std::string> objectsOptions; // Denumirea obiectelor care vor fi folosite pentru ciclare

        float x = 0;
        float y = 0.5f;
        float z = -1;
        float speed = 1;
    };
}   // namespace m1
