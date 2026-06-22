#pragma once

#include <string>
#include <unordered_map>

class UI;
class Square;
class Texture;
class TextTexture;
class GameStateManager;

class UIManager {
    public:
    std::unordered_map<std::string, UI*> uiMap;

    void RenderUIs();
    void DestroyUIs();

    //레이아웃 관련 변수
    int mTopPanelH{60};
};

class UI {
    public:

    UI() = default;
    UI(Square*, std::string);

    float ClipDesiredTextLength(TextTexture &ttexture);

    virtual void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY);

    bool mIsUIUpdate{true}; //ui 업데이트 플래그 변수, 생성될 때 참이면 한번 업데이트 하고 거짓으로 바뀐다.

    virtual void RenderOnUpdate(); //플래그에 따라 업데이트하는 메서드
    virtual void Render(); //그냥 막 업데이트하는 녀석

    void SetPadding(float padding);
    float GetPadding();

    Square* mUIFrame{nullptr};
    std::string mUIText;

    SDL_Texture* mTexture{nullptr}; //여기다 저장해놓고 하나만 렌더링하게 한다.
    Texture* mMyTexture{nullptr};

    float mPadding = 10.f;
    private:
};

//버튼 기능 타입
enum class BtnType {
    Title,
    OverMap, City
};

//일단은 이벤트 핸들링이 안되는 텍스트 ui
class TextUI : public UI {
    public:
    TextUI(int x, int y, int width, int height, std::string text);
    TextUI(int x, int y, TTF_Font* font, std::string text);

    TTF_Font* mFont{nullptr};

    void RenderOnUpdate() override;

    private:
    int mX, mY, mW, mH;
};
//일반적인 버튼
class Button : public UI {
    public:
    Button(Square* frame, std::string text, BtnType BtnType);

    void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY) override;

    BtnType mType;
};

//맵 타일용 ui
class MaptileUI : public UI {
    public:
    MaptileUI();

    void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY) override;
};

//이미지를 로딩해서 쓰는 ui
class IconUI : public UI {
    public:
    IconUI(int x, int y, int width, int height, std::string path);

    void RenderOnUpdate() override;
    void HandleEvent(SDL_Event& e, GameStateManager& gs, float mouseX, float mouseY) override;

    private:
    int mX, mY, mW, mH;
};