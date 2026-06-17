#pragma once

#include <string>
#include <unordered_map>

class UI;
class Square;
class TextTexture;
class GameStateManager;

class UIManager {
    public:
    std::unordered_map<std::string, UI*> uiMap;

    void RenderUIs();
    void DestroyUIs();
};

class UI {
    public:

    UI() = default;
    UI(Square*, std::string);

    float ClipDesiredTextLength(TextTexture &ttexture);

    virtual void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY);

    bool mIsUIUpdate{true}; //ui 업데이트 플래그 변수, 생성될 때 참이면 한번 업데이트 하고 거짓으로 바뀐다.
    void RenderOnUpdate(); //플래그에 따라 업데이트하는 메서드
    void Render(); //그냥 막 업데이트하는 녀석

    void SetPadding(float padding);
    float GetPadding();

    Square* mUIFrame{nullptr};
    std::string mUIText;
     SDL_Texture* mTexture{nullptr}; //여기다 저장해놓고 하나만 렌더링하게 한다.

    float mPadding = 10.f;
    private:
};

//버튼 기능 타입
enum class BtnType {
    Start
};

class Button : public UI {
    public:
    Button(Square* frame, std::string text, BtnType BtnType);

    void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY) override;

    BtnType mType;
};