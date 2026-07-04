#pragma once

#include <string>
#include <unordered_map>

//전방선언 리스트
class ObjectManager;
class UI;
class ToolTip;
class Square;
class Texture;
class GameStateManager;
class MapTile;

class UIManager {
    public:
    UIManager();
    std::unordered_map<std::string, UI*> uiMap;

    void RenderUIs();
    void DestroyUIs();

    //툴팁
    ToolTip* mToolTip{nullptr};

    //레이아웃 관련 변수
    int mTopPanelH{60};
};

class UI {
    public:

    UI() = default;
    UI(Square* frame, std::string text);

    virtual void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY);

    bool mIsRender{false}; //렌더링 자체를 제어하는 플래그
    bool mIsUIUpdate{true}; //ui 업데이트 플래그 변수, 생성될 때 참이면 한번 업데이트 하고 거짓으로 바뀐다.

    virtual void RenderOnUpdate(); //플래그에 따라 업데이트하는 메서드

    std::string mUIText = "null";

    Square* mUIFrame{nullptr};

    SDL_Texture* mTexture{nullptr}; //여기다 저장해놓고 하나만 렌더링하게 한다.
    Texture* mMyTexture{nullptr};

    float mPadding = 10.f;
    private:
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

//버튼 기능 타입
enum class BtnType {
    Title,
    OverMap, City
};

//일반적인 버튼
class Button : public UI {
    public:
    Button(Square* frame, std::string text, BtnType BtnType);

    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY) override;

    BtnType mType;
};

//툴팁
class ToolTip : public UI {
    public:
    ToolTip();
    void Destroy();

    void SetRefInfo(int x, int y, int w, int h);

    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY) override;
    void RenderOnUpdate() override;
    void Render();

    float mX, mY; //위치
    int mRefX, mRefY, mRefW, mRefH {-1}; //참조용 좌표(실제 오브젝트 위치)
    int mPrevX, mPrevY, mPrevW, mPrevH {-1}; //저장할 이전 좌표
};

//이미지를 로딩해서 쓰는 ui
class IconUI : public UI {
    public:
    IconUI(int x, int y, int width, int height, std::string path);

    void RenderOnUpdate() override;
    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY) override;

    private:
    int mX, mY, mW, mH;
};