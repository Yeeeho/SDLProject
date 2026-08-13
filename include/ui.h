#pragma once

#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "item/item_enum.h"

//전방선언 리스트
enum class EqType;
struct GameContext;
class ObjectManager;
class UIManager;
class GameStateManager;
class Texture;
class Square;
struct Point;
class UI;
class TTFWord;
class Grid;
class Map;
class Camera;
class Pawn; class Entity;
class Skill;

class UI {
    public:
    UI() = default;

    virtual void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);

    virtual void StoreTexture(); //텍스처 캐싱
    virtual void RenderStoredTex(); //캐시 텍스처 렌더링
    void Render();

    SDL_Texture* mTempTex {nullptr}; //여기다 저장해놓고 업데이트시에 이걸 렌더링
    
    int mPadding = 10;

    bool mIsRender{false}; //렌더링 자체를 제어하는 플래그
    bool mIsRenderUpdate{true}; //ui 업데이트 플래그 변수, 생성될 때 참이면 한번 업데이트 하고 거짓으로 바뀐다.
    bool mCanHandleEvent {false}; //이벤트 핸들링 가능?

    int mX {0}, mY {0}, mW {0}, mH {0};
    int mOffsetX {0}, mOffsetY {0};

    private:
};

//텍스트만 있는 ui
class TextUI {
    public:
    TextUI(float x, float y);

    int GetHeight();

    void ClearTexts();

    void AddWord(TTFWord word);
    void ProcessAndAddText(std::string text, SDL_Color color, TTF_Font* font);

    void RenderWords();
    void RenderAtLine(const TTFWord& text);
    void NewLine(TTF_Font* font);
    void AddSpace(TTF_Font* font);
    
    float mX, mY;
    int mPadding {10};
    int mLineSpacing {4};
    int mTotalWidth {0}, mTotalHeight {0}; //텍스처 렌더링 좌표 계산용

    std::map<int,TTFWord> mTexts;
    private:
    int mWordId {0};
};

// 프레임이 있는 text ui
class FramedTUI {
    public:
    FramedTUI(int x, int y, int w, int h);

    void AddWordAndProcess(TTFWord word);
    void AddWord(TTFWord word);
    void ClearText();
    //렌더링
    void Render();
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);

    TextUI* mTui {nullptr};

    int mPadding {10};
    int mTotalW {0};
    int mTotalH {0};
    int mX {0}, mY {0};
    int mW {0}, mH {0};

    bool mIsRender {false};
    bool mIsUIUpdate {true};
    private:
};

//버튼 기능 타입
enum class BtnType {
    Default,
    Title, 
    NewGame, LoadGame,
    OverMap, SubMap, City,
    SubMapTurnOver, OverMapTurnOver, CityMapTurnOver
};

//일반적인 버튼
class Button : public UI {
    public:
    Button(int x, int y, int w, int h, std::string text, BtnType BtnType);

    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY) override;
    bool IsMouseIn(float mx, float my);

    void RenderThings();
    void StoreTexture() override;

    TextUI* mTui {nullptr};

    BtnType mType;
};

//툴팁
class ToolTip : public UI {
    public:
    ToolTip();
    void Destroy();

    void ClearContent();

    void SetToolTipFrame();

    void SetRefInfo(int x, int y, int w, int h); //참조 좌표 설정

    //업데이트
    void CheckUpdate(); //업데이트 로직에서 업데이트 유무를 검사함
    //렌더링 메서드
    void RenderThings(float x, float y);
    void StoreTexture();
    void RenderStoredTex();
    void Render();

    TextUI* mTui {nullptr}; //텍스트
    Square* mUIFrame {nullptr}; //프레임

    float mX {0}, mY {0}; //위치
    int mW {0}, mH {0}; //크기
    int mRefX {-1}, mRefY {-1}, mRefW {-1}, mRefH {-1}; //참조용 좌표(실제 오브젝트 위치)
    int mPrevX {-1}, mPrevY {-1}, mPrevW {-1}, mPrevH {-1}; //저장할 이전 좌표
};

//이미지를 로딩해서 쓰는 ui
class IconUI {
    public:
    IconUI(int x, int y, int width, int height, std::string path);

    void Render();
    void RenderByCam(Camera* cam);
    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);

    void SetDimension(int x, int y, int w, int h);

    Texture* mTex {nullptr};

    bool mIsRender {false};

    private:
    int mX {0}, mY {0}, mW {0}, mH {0};
};

class ScenarioManager;

class DialogueUI : public UI {
    public:
    DialogueUI(float x, float y);

    SDL_Texture* mBasicTex {nullptr};
    
    //이벤트 핸들링    
    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);
    //업데이트 관련
    void Update(ScenarioManager& scm);
    bool mIsUpdate {false};
    //렌더링
    void StoreBasicTex(); //기본 텍스처 저장
    void RenderOnUpdate();

    //대화창 설정
    void SetUI(Texture* pic, TTFWord name, std::string text);
    void SetUI(Texture* pic);
    void SetUI(std::string text);

    float mX {-1}, mY {-1};

    //ui컴포넌트
    Square* mPanel {nullptr};

    Texture* mSpeakerBg {nullptr};
    Texture* mSpeakerImg {nullptr};
    Texture* mSpkrBlankImg {nullptr}; //디폴트용 화자 텍스처
    Texture* mSpeakerFrame {nullptr};

    Texture* mDialogueBodyBg {nullptr};
    FramedTUI* mDialogueBody {nullptr};
    Texture* mDialogueBodyFrame {nullptr};
};
class MapTile;
//타일 강조 ui
class TileHLUI {
    public:
    TileHLUI();
    ~TileHLUI();

    void SetTileIds(std::vector<int> ids);
    void ClearTileIds();

    void Update();

    void RenderBetweenTiles(Map* map);
    bool mIsRenderBetweenTiles {false};

    Texture* mHighlight {nullptr};

    private:
    std::vector<int> mTIds;
};

//캐릭터 시트용 ui
struct SlotInfo {
    public:
    EqType mEqType {EqType::All};
    int mId {0};
    std::string mInfo {""};
    int x {0};
    int y {0};
};

class InventoryUI : public UI {
    public:
    InventoryUI(int x, int y, int offsetX, int offsetY, int w, int h, int tileLen, GameContext* gc);

    void Activate(Pawn* p);
    void Deactivate();

    void LoadEqToolTip(SlotInfo& si);

    void HandleEvent(SDL_Event& e, float mx, float my);
    void HandleEqSlotEvent(SDL_Event& e, SlotInfo si, float mx, float my);
    bool mMouseIn {false};

    void RenderThings();
    void RenderEqSlots(Texture& t);
    void RenderEqSlot(Texture& t, int x, int y, std::string info);

    std::multimap<EqType, SlotInfo> mSlotInfos;
    ToolTip* mToolTip {nullptr};

    GameContext* mGc {nullptr};

    Grid* mGrid {nullptr};
    int mPrevEqIdx {0};    
};

class CharacterSkillUI : public UI {
    public:
    CharacterSkillUI(int x, int y, int w, int h);
    
    void Activate(Entity* ent);
    void Deactivate();
    
    void HandleEvent(SDL_Event& e, GameContext* gc, float mx, float my);
    void HandleSkillListEvent(SDL_Event& e, GameContext* gc, float mx, float my);
    
    void UpdateUI(Entity* ent);
    void UpdateSkillDesc(int idx, GameContext* gc);
    
    void RenderThings();
    
    std::vector<FramedTUI*> mSkillList; //스킬 리스트
    FramedTUI* mSkillDesc {nullptr}; //스킬 설명
};

enum class CharacterSheetIdx {
    Skill, Info, Inv
};

class CharacterSheetUI : public UI {
    public:
    CharacterSheetUI(int x, int y, int w, int h, GameContext* gc);

    void Activate(Pawn* pc);
    void Deactivate();

    void HandleEvent(SDL_Event& e, GameContext& gc, float mx, float my);

    void StoreTexture() override;

    GameContext* mGc {nullptr};

    Button* mInvTab {nullptr};
    Button* mSkillTab {nullptr};

    CharacterSkillUI* mCsUI {nullptr};
    InventoryUI* mInvUI {nullptr};
    CharacterSheetIdx mTabIdx {CharacterSheetIdx::Inv};
};

class QuickSkillUI : public UI{
    public:
    QuickSkillUI(int x, int y, int w, int h, GameContext& gc);
    
    void AddSkill(Skill* skill);
    
    void StoreTexture() override;
    
    void HandleEvent(SDL_Event& e, GameContext& gc, Map* map, float mouseX, float mouseY);
    void Activate(GameContext& gc, Map* map, Pawn* pawn);
    void Deactivate(GameContext& gc, Map* map);
    
    GameContext* mGc {nullptr};
};

enum class BCUIMainStatIdx {
    Name = 0, CurHp = 4, MaxHp = 5, CurSp = 9, MaxSp = 10, CurAp = 14, MaxAp = 15
};

class BottomCharacterUI : public UI{
    public:
    BottomCharacterUI(int x, int y, int w, int h);

    void UpdateUI(Entity* ent);

    void StoreTexture() override;

    FramedTUI* mMainStat {nullptr}; //hp 등을 보여주는 ui컴포넌트
    FramedTUI* mStatEffect {nullptr}; //상태이상들을 보여주는 ui 컴포넌트
};

class LogUI : public UI {
    public:
    LogUI(int x, int y, int w, int h);

    void AddMessage(std::string message, SDL_Color c);

    void StoreTexture() override;

    int mTotalH {0};
    
    FramedTUI* mBody {nullptr};
};

class UIManager {
    public:
    UIManager(GameContext& gc);

    //참고용 컨텍스트 객체
    GameContext* mGc {nullptr};

    //ui 컨테이너
    std::map<std::string, UI*> uiMap;
    std::unordered_map<std::string, Square*> mPanels;
    
    void InitTopBar(); //탑 바를 초기화하는 녀석
    void InitUIs(); //게임 상태마다 초기화 할 ui들을 초기화

    //파괴자
    void DestroyUIs();
    
    //이벤트 핸들링
    void HandleUIEvent(SDL_Event& e, GameContext& gc, float mouseX, float mousey);
    void HandleMapUIEvent(SDL_Event& e, GameContext& gc, Map* map, float mx, float my);

    //렌더링
    void RenderUIs();
    void RenderMapUIs(Map* map); //맵상에 있을 ui들 렌더링
    
    //툴팁 관련
    ToolTip* mToolTip{nullptr}; //툴팁
    Grid* mToolTipGrid {nullptr};

    void LoadInvToolTip(Grid* grid, int tileId);
    void LoadMapToolTip(Map* map, int tileId);
    void UpdateGridToolTip(Grid* grid);

    void HandleMapToolTipEvent(SDL_Event& e, GameStateManager& gsm, float mouseX, float mouseY);
    bool mCanHandleToolTip {true};
    void RenderMapToolTip(Grid* grid);
    
    //맵 관련 ui 객체
    IconUI* mFocusIcon {nullptr}; //맵 타일 포커스 아이콘
    TileHLUI* mTileHLUI {nullptr}; //맵 타일 강조 ui

    //로그
    LogUI* mLogUI {nullptr};
    //대화창
    DialogueUI* mDialogueUI {nullptr};
    //스킬 퀵슬롯
    QuickSkillUI* mQSUI {nullptr};
    //캐릭터 하단 간략화 정보창
    BottomCharacterUI* mBCUI {nullptr};

    //캐릭터 정보창
    CharacterSheetUI* mCharacterSheet {nullptr};
    //턴 종료 버튼
    Button* mTurnOverBtn {nullptr};
    bool mWasMouseOnMap {false};
    
    //레이아웃 관련 변수
    //TODO: 이런거는 설정 파일로 빼라
    int mTopPanelH{60};
};

class UIHelper {
    public:
    void AddEntityData(TextUI* tui, Entity* ent);
};
