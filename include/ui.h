#pragma once

#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

//전방선언 리스트
class ObjectManager;
class TTFWord;
class Map;
class UI;
class ToolTip;
class FramedTUI;
class Square;
class Texture;
class GameStateManager;

class UIManager {
    public:
    UIManager();

    //ui 컨테이너
    std::unordered_map<std::string, UI*> uiMap;
    std::unordered_map<std::string, FramedTUI*> ftuiMap;
    std::unordered_map<std::string, Square*> mPanels;
    
    void InitTopBar(); //탑 바를 초기화하는 녀석
    void InitUIs(); //게임 상태마다 초기화 할 ui들을 초기화

    //파괴자
    void DestroyUIs();

    //이벤트 핸들링
    void HandleUIEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mousey);
    
    //렌더링
    void RenderUIs();
    void RenderMapToolTip(Map* map);

    // 맵 툴팁 관련
    void LoadMapToolTip(Map* map, int tileId);
    void UpdateMapToolTip(Map* map);
    void HandleMapToolTipEvent(SDL_Event& e, GameStateManager& gsm, Map* map, float mouseX, float mouseY);

    bool mWasMouseOnMap {false};

    //툴팁
    ToolTip* mToolTip{nullptr};

    //레이아웃 관련 변수
    int mTopPanelH{60};
};

class UI {
    public:
    UI() = default;
    UI(Square* frame, std::string text);
    void Destroy();

    virtual void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY);

    bool mIsRender{true}; //렌더링 자체를 제어하는 플래그
    bool mIsUIUpdate{true}; //ui 업데이트 플래그 변수, 생성될 때 참이면 한번 업데이트 하고 거짓으로 바뀐다.

    virtual void RenderOnUpdate(); //플래그에 따라 업데이트하는 메서드

    std::string mUIText = "null";

    Square* mUIFrame{nullptr}; //프레임

    //비주얼
    void SetFrameColor(SDL_Color fillcolor, SDL_Color linecolor);

    SDL_Texture* mTempTex {nullptr}; //여기다 저장해놓고 업데이트시에 이걸 렌더링
    Texture* mMyTexture {nullptr};

    float mPadding = 10.f;
    private:
};

//텍스트만 있는 ui
class TextUI : public UI {
    public:
    TextUI(float x, float y);

    void ProcessAndAddText(std::string text, SDL_Color color, TTF_Font* font);

    void RenderWords();
    void RenderAtLine(const TTFWord& text);
    void NewLine(TTF_Font* font);
    void AddSpace(TTF_Font* font);

    void RenderOnUpdate() override;
    
    float mX, mY;
    int mLineSpacing = 4;
    int mTotalWidth {0}, mTotalHeight {0}; //텍스처 렌더링 좌표 계산용

    std::vector<TTFWord> mTexts;
};
// 프레임이 있는 text ui
class FramedTUI : public UI {
    public:
    FramedTUI(int x, int y, int w, int h);

    void AddWord(TTFWord word);
    void ClearText();

    //렌더링
    void RenderOnUpdate() override;
    void Render();
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY) override;

    TextUI* mTui {nullptr};

    int mX {0}, mY {0};
    int mW {0}, mH {0};
};

//버튼 기능 타입
enum class BtnType {
    Title, 
    NewGame, LoadGame,
    OverMap, SubMap, City
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

    void SetToolTipFrame();

    void SetRefInfo(int x, int y, int w, int h); //참조 좌표 설정

    //업데이트
    void CheckUpdate(); //업데이트 로직에서 업데이트 유무를 검사함
    //이벤트 핸들러
    void HandleEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY);
    //렌더링 메서드
    void RenderOnUpdate() override;
    void Render();

    TextUI* mTui {nullptr}; //텍스트

    float mX {0}, mY {0}; //위치
    int mW, mH; //크기
    int mRefX {-1}, mRefY {-1}, mRefW {-1}, mRefH {-1}; //참조용 좌표(실제 오브젝트 위치)
    int mPrevX {-1}, mPrevY {-1}, mPrevW {-1}, mPrevH {-1}; //저장할 이전 좌표
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