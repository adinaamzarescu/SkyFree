#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"
#include <Core/GPU/Mesh.h>

#include "components/text_renderer.h"

#include <vector>
#include <iostream>

using namespace std;

namespace m1
{
    struct Object
    {
        glm::vec3 position;
        float angle;
        bool collected, visible;
        float distanceToPlayer;

        Object() : position(0, 0, 0), angle(0), collected(false), visible(false), distanceToPlayer(0) {}

    };
    class Tema3 : public gfxc::SimpleScene
    {
    public:
        Tema3();
        ~Tema3();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void DrawScene();
        void Restart();
        bool IsVisible(const glm::vec3& position);
        void GameOver();


        Shader* shader;

        std::unordered_map<std::string, Texture2D*> mapTextures;
        // game objects
        Object player;
        vector<Object> trees, bigStones, smallStones, poles, gifts;


        // game state
        int score = 0;
        bool finished = false;

        // Final text
        void DrawHUD();
    private:
        gfxc::TextRenderer* textRenderer = NULL;

        // Colors
        const glm::vec3 kTextColor = NormalizedRGB(25, 25, 112);
        const glm::vec3 newTextColor = NormalizedRGB(199, 21, 133);
    };
}   // namespace m1