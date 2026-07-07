#pragma once

#include <string>
#include <unordered_map>

//전방선언 리스트
class ObjectManager;
class TTFWord;
class UI;
class ToolTip;
class Square;
class Texture;
class GameStateManager;

class UIManager {
    public:
    UIManager();
    std::unordered_map<std::string, UI*> uiMap;

    void RenderUIs();
    void DestroyUIs();

    // 맵 툴팁 관련
    void UpdateMapToolTip(const ObjectManager& objm);
    void HandleMapToolTipEvent(SDL_Event& e, GameStateManager& gsm, ObjectManager& objm, float mouseX, float mouseY);

    //이거는 물리엔진 클래스를 따로 만들어서 거기에 넣어야할듯
    bool MouseCollisionCheck(float mouseX, float mouseY, float mX, float mY, float mW, float mH); //마우스 충돌 검사
    //mouseover 중인 타일의 id를 반환하는 함수
    int WhatTileOnPoint(float pointX, float pointY, int mX, int mY, int xTiles, int yTiles, int xTileLen, int yTileLen);

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

//텍스트만 있는 ui
class TextUI : public UI {
    public:
    TextUI(float x, float y);

    void RenderWords();
    void RenderAtLine(const TTFWord& text);
    void NewLine(TTF_Font* font);
    void AddSpace(TTF_Font* font);

    void RenderOnUpdate() override;
    
    float mX, mY;
    int mTotalWidth {0}, mTotalHeight {0}; //텍스처 렌더링 좌표 계산용

    std::vector<TTFWord> mTexts;
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

    void AppendText(std::string text, SDL_Color color, TTF_Font* font); //ui에 텍스트 데이터를 추가함.
    void SetRefInfo(int x, int y, int w, int h); //참조 좌표 설정

    void CheckUpdate(); //업데이트 로직에서 업데이트 유무를 검사함

    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY);
    void RenderOnUpdate() override;
    void Render();

    TextUI* mTui {nullptr};

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