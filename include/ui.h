#pragma once

#include <string>
#include <unordered_map>

class UI;
class Square;
class TextTexture;

class UIManager {
    public:
    std::unordered_map<std::string, UI*> uiMap;

    void InitUIs();

    void RenderUIs();
};

class UI {
    public:

    UI() = default;
    UI(Square*, std::string);

    float ClipDesiredTextLength(TextTexture &ttexture);

    void Render(bool& isUIUpdate);
    void Render();

    void SetPadding(float padding);
    float GetPadding();

    Square* mUIFrame{nullptr};
    std::string mUIText;

    private:
    float mPadding = 10;
};

class Button : public UI {
    public:
    Button(Square*, std::string);

    void HandleEvent(SDL_Event& e, float mouseX, float mousY);
};