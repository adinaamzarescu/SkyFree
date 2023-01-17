#include "lab_m1/Tema3/Tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow.jpg").c_str(), GL_REPEAT);
        mapTextures["snowTexture"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "player.jpg").c_str(), GL_REPEAT);
        mapTextures["playerTexture"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "ski.jpg").c_str(), GL_REPEAT);
        mapTextures["skiTexture"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "bark.jpg").c_str(), GL_REPEAT);
        mapTextures["barkTexture"] = texture;
    }
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "needles.jpg").c_str(), GL_REPEAT);
        mapTextures["needlesTexture"] = texture;
    }
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "stone.jpg").c_str(), GL_REPEAT);
        mapTextures["stoneTexture"] = texture;
    }
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "stone.jpg").c_str(), GL_REPEAT);
        mapTextures["poleTexture"] = texture;
    }
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "gift.jpg").c_str(), GL_REPEAT);
        mapTextures["giftTexture"] = texture;
    }

    // Load meshes
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "models"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "models"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("cone");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "models"), "cone.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "models"), "quad.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program
    {
       shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Show text for score
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 30);

    if (!finished)
        Restart();
}


void Tema3::FrameStart()
{
    // update viewport
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    // clear window
    glClearColor(0.5f, 0.6f, 0.7f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema3::Update(float deltaTimeSeconds)
{
    if (finished) {
        GameOver();
        return;
    }

    // get player direction vector
    glm::mat4 matrix = glm::rotate(glm::mat4(1), player.angle, glm::vec3(0, 1, 0));
    glm::vec3 playerDirection = matrix * glm::vec4(0, 0, 1, 1);

    // update player position
    const float playerSpeed = 8;
    player.position += playerDirection * playerSpeed * deltaTimeSeconds;

    // update visibility of all objects
    for (auto& tree : trees)
        tree.visible = IsVisible(tree.position);
    for (auto& stone : smallStones)
        stone.visible = IsVisible(stone.position);
    for (auto& stone : bigStones)
        stone.visible = IsVisible(stone.position);
    for (auto& pole : poles)
        pole.visible = IsVisible(pole.position);
    for (auto& gift : gifts)
        gift.visible = IsVisible(gift.position);

    // check for collisions with gift boxes
    for (auto& gift : gifts)
    {
        // ignore invisible or collected gifts
        if (!gift.visible || gift.collected)
            continue;

        // distance-based collision detection
        float distance = glm::distance(gift.position * glm::vec3(1, 0, 1), player.position);
        if (distance < 1)
        {
            // hide collided gift box and increment score
            gift.collected = true;
            score++;
        }
    }

    // check for collisions with trees
    for (auto& tree : trees)
    {
        // ignore invisible trees
        if (!tree.visible)
            continue;

        // distance-based collision detection
        float distance = glm::distance(tree.position * glm::vec3(1, 0, 1), player.position);
        if (distance < 1)
        {
            // Score becomes 0 and game restarts
            finished = true;
            GameOver();
        }
    }
    // check for collisions with poles
    for (auto& pole : poles)
    {
        // ignore invisible poles
        if (!pole.visible)
            continue;

        // distance-based collision detection
        float distance = glm::distance(pole.position * glm::vec3(1, 0, 1), player.position);
        if (distance < 1)
        {
            // Score becomes 0 and game restarts
            finished = true;
            GameOver();
        }
    }
    // check for collisions with small stones
    for (auto& smallStone : smallStones)
    {
        // ignore invisible smallStones
        if (!smallStone.visible)
            continue;

        // distance-based collision detection
        float distance = glm::distance(smallStone.position * glm::vec3(1, 0, 1), player.position);
        if (distance < 1)
        {
            // Score becomes 0 and game restarts
            finished = true;
            GameOver();
        }
    }

    // check for collisions with big stones
    for (auto& bigStone : bigStones)
    {
        // ignore invisible bigStones
        if (!bigStone.visible)
            continue;

        // distance-based collision detection
        float distance = glm::distance(bigStone.position * glm::vec3(1, 0, 1), player.position);
        if (distance < 1)
        {
            // Score becomes 0 and game restarts
            finished = true;
            GameOver();
        }
    }

    // check if out of bounds or finished
    if (fabs(player.position.x) > 50 || player.position.z > 525)
        finished = true;
}


void Tema3::FrameEnd()
{
    glm::ivec2 resolution = window->GetResolution();
    if (resolution.x == 0 || resolution.y == 0)
        return;
    if (!finished)
        DrawScene();
}

void m1::Tema3::DrawScene()
{

    // set view matrix with camera position and orientation
    glm::vec3 eyePosition = player.position + glm::vec3(0, 15, 10);
    glm::vec3 upDirection(0, 1, 0);
    glm::mat4 viewMatrix = glm::lookAt(eyePosition, player.position, upDirection);
    glUniformMatrix4fv(shader->GetUniformLocation("View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // set perspective projection matrix with camera properties
    float fieldOfView = RADIANS(45.0f);
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    glm::mat4 projectionMatrix = glm::perspective(fieldOfView, window->props.aspectRatio, nearPlane, farPlane);
    glUniformMatrix4fv(shader->GetUniformLocation("Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));


    float tileSize = 100;
    for (int i = 0; i <= 5; i++)
    {
        glm::mat4 snowMatrix = glm::translate(glm::mat4(1), glm::vec3(0, -0.5f, i * tileSize));
        snowMatrix = glm::scale(snowMatrix, glm::vec3(tileSize, 1, tileSize));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], snowMatrix, mapTextures["snowTexture"]);
    }

    // get player matrix
    glm::mat4 playerMatrix = glm::translate(glm::mat4(1), player.position);
    playerMatrix = glm::rotate(playerMatrix, player.angle, glm::vec3(0, 1, 0));

    // draw player body as box
    glm::mat4 bodyMatrix = glm::translate(playerMatrix, glm::vec3(0, 0.5f, 0));
    bodyMatrix = glm::scale(bodyMatrix, glm::vec3(1, 1, 1));
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], playerMatrix, mapTextures["playerTexture"]);

    // draw 2 skis as boxes
    for (int i = 0; i < 2; i++)
    {
        glm::mat4 skiMatrix = glm::translate(playerMatrix, glm::vec3(0.2f - 0.4f * i, 0, 0));
        skiMatrix = glm::scale(skiMatrix, glm::vec3(0.1f, 0.1f, 2));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], skiMatrix, mapTextures["skiTexture"]);
    }

    // draw tree trunks as boxes
    for (const auto& tree : trees)
    {
        // ignore invisible trees
        if (!tree.visible)
            continue;

        glm::mat4 trunkMatrix = glm::translate(glm::mat4(1), tree.position);
        trunkMatrix = glm::scale(trunkMatrix, glm::vec3(0.3f, 2, 0.3f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], trunkMatrix, mapTextures["barkTexture"]);
    }


    // draw tree needles as cones
    for (const auto& tree : trees)
    {
        // ignore invisible trees
        if (!tree.visible)
            continue;

        for (int i = 0; i < 2; i++)
        {
            glm::mat4 needlesMatrix = glm::translate(glm::mat4(1), tree.position + glm::vec3(0, 2.5f + i, 0));
            needlesMatrix = glm::scale(needlesMatrix, glm::vec3(i == 0 ? 1.0f : 0.7f));
            RenderSimpleMesh(meshes["cone"], shaders["LabShader"], needlesMatrix, mapTextures["needlesTexture"]);
        }
    }

    // draw stones
    for (const auto& stone : smallStones)
    {
        // ignore invisible stones
        if (!stone.visible)
            continue;

        // draw stone as 3 spheres
        for (int i = 0; i < 3; i++)
        {
            glm::mat4 stoneMatrix = glm::translate(glm::mat4(1), stone.position + glm::vec3(i * 0.5f, 0, 0));
            stoneMatrix = glm::scale(stoneMatrix, glm::vec3(i == 1 ? 0.8f : 1.0f));
            RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], stoneMatrix, mapTextures["stoneTexture"]);
        }

    }

    for (const auto& stone : bigStones)
    {
        // ignore invisible stones
        if (!stone.visible)
            continue;

        // draw stone as 3 spheres
        for (int i = 0; i < 4; i++)
        {
            glm::mat4 stoneMatrix = glm::translate(glm::mat4(1), stone.position + glm::vec3(i * 0.5f, 0, 0));
            stoneMatrix = glm::scale(stoneMatrix, glm::vec3(i == 1 ? 0.8f : 1.0f));
            RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], stoneMatrix, mapTextures["stoneTexture"]);
        }

    }

    // draw poles
    for (const auto& pole : poles)
    {
        // ignore invisible poles
        if (!pole.visible)
            continue;

        // draw pole as 2 boxes
        glm::mat4 poleMatrix = glm::translate(glm::mat4(1), pole.position);
        poleMatrix = glm::scale(poleMatrix, glm::vec3(0.2f, 5, 0.2f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], poleMatrix, mapTextures["poleTexture"]);
        poleMatrix = glm::translate(glm::mat4(1), pole.position + glm::vec3(0, 2.5f, 0));
        poleMatrix = glm::scale(poleMatrix, glm::vec3(2, 0.3f, 0.3f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], poleMatrix, mapTextures["poleTexture"]);
    }

    // draw gifts
    for (const auto& gift : gifts)
    {
        // ignore invisible or collected gifts
        if (!gift.visible || gift.collected)
            continue;

        // draw gift as box
        glm::mat4 giftMatrix = glm::translate(glm::mat4(1), gift.position);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], giftMatrix, mapTextures["giftTexture"]);
    }
 
}

void m1::Tema3::Restart()
{
    // init player
    player = Object();
    player.position = glm::vec3(0, 0, 0);
    player.angle = 0;


    // create random trees
    int mapLength = 500;
    int mapWidth = 30;
    trees.clear();
    trees.resize(50);
    for (unsigned int i = 0; i < trees.size(); i++)
    {
        trees[i].position.x = float(rand() % mapWidth - mapWidth / 2);
        trees[i].position.y = 1;
        trees[i].position.z = (i + 1.00f) / trees.size() * mapLength;
    }


    // create random small stones
    smallStones.clear();
    smallStones.resize(50);
    for (unsigned int i = 0; i < smallStones.size(); i++)
    {
        smallStones[i].position.x = float(rand() % mapWidth / 2 - mapWidth / 4);
        smallStones[i].position.y = 0;
        smallStones[i].position.z = (i + 1.25f) / smallStones.size() * mapLength;
    }
    // create random big stones
    bigStones.clear();
    bigStones.resize(50);
    for (unsigned int i = 0; i < bigStones.size(); i++)
    {
        bigStones[i].position.x = float(rand() % mapWidth / 2 - mapWidth / 4);
        bigStones[i].position.y = 0;
        bigStones[i].position.z = (i + 1.25f) / bigStones.size() * mapLength;
    }

    // create random poles
    poles.clear();
    poles.resize(50);
    for (unsigned int i = 0; i < poles.size(); i++)
    {
        poles[i].position.x = float(rand() % mapWidth / 2 - mapWidth / 4);
        poles[i].position.y = 2.5f;
        poles[i].position.z = (i + 1.50f) / poles.size() * mapLength;
    }

    // create random gifts
    gifts.clear();
    gifts.resize(50);
    for (unsigned int i = 0; i < gifts.size(); i++)
    {
        gifts[i].position.x = float(rand() % mapWidth / 2 - mapWidth / 4);
        gifts[i].position.y = 0.5f;
        gifts[i].position.z = (i + 1.75f) / gifts.size() * mapLength;
    }

    // init game state
    score = 0;
    finished = false;
}

bool m1::Tema3::IsVisible(const glm::vec3& position)
{
    return fabs(position.z - player.position.z) < 20;
}


void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));


    GLint locTime = glGetUniformLocation(shader->program, "time");

    if (texture1)
    {
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    }

    if (texture2)
    {
        // - activate texture location 1
        glActiveTexture(GL_TEXTURE1);
        // - bind the texture2 ID
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        // - send the uniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}



void Tema3::OnInputUpdate(float deltaTime, int mods)
{

}


void Tema3::OnKeyPress(int key, int mods)
{
}


void Tema3::OnKeyRelease(int key, int mods)
{
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (finished)
        return;

    // set player angle using X mouse position
    glm::ivec2 resolution = window->GetResolution();
    player.angle = mouseX * 2.0f / resolution.x - 1;
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_2)
        Restart();
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}


void Tema3::DrawHUD()
{
    // Show the score
    const float scoreTop = 300;
    const float scoreWidth = 350;
    textRenderer->RenderText("GAME OVER", scoreWidth, scoreTop - 80, 3, kTextColor);
    textRenderer->RenderText("Final score =", scoreWidth, scoreTop, 2, kTextColor);

    // Convert float to string
    stringstream ss;
    ss << score;
    string str = ss.str();
    textRenderer->RenderText(str, scoreWidth + 500, scoreTop, 2, kTextColor);

    textRenderer->RenderText("Left Click to start again", scoreWidth + 30, scoreTop + 200, 1, newTextColor);
}

void Tema3::GameOver() {
    DrawHUD();
}