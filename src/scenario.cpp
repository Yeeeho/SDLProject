#include "pch.h"

#include "system.h"
#include "game_context.h"
#include "game_state.h"
#include "game_json.h"
#include "game_object.h"
#include "turn.h"
#include "map.h"
#include "city.h"
#include "scenario.h"
#include "text.h"
#include "texture.h"
#include "ui.h"
#include "entity.h"

void Scenario::LoadScenarioData(GameContext& gc)
{
}

void Scenario::LoadSubMap(GameContext& gc)
{
}

void Scenario::LoadOverMap(GameContext& gc)
{
}

void Scenario::LoadCityMap(GameContext& gc)
{
}

void Scenario::Update(GameContext& gc)
{
}

void Scenario::UpdateScenario(GameContext& gc)
{
    if (!mIsScUpdate) return;

    SDL_Log("update scenario");

    JsonHelper jh;
    json section = mData["items"][mScProgress];

    std::string sType = section["type"].get<std::string>();
    std::string message = "scenario type: " + sType;
    SDL_Log(message.c_str());

    if (sType == "dialogue") {
        //어느 섹션의 대화인지를 찾아서 가져온다.
        jh.LoadJsonFile(mDialogue, section["path"].get<std::string>());

        std::string dSection = section["d_section"].get<std::string>();
        mDialogueSection = mDialogue["items"][dSection];
    }
    else if (sType == "continue_dialogue") {
        //로딩을 하지 않고 섹션만 다시 찾는다.
        std::string dSection = section["d_section"].get<std::string>();
        mDialogueSection = mDialogue["items"][dSection];

        mIsDialogueUpdate = true;
        gc.mUim->mDialogueUI->mIsRenderUpdate = true;
    }
    else if (sType == "static_spawn") {
        Map* map {nullptr};
        std::string where = section["where"].get<std::string>();
        //어떤 맵에 스폰할지 결정한다.
        if (where == "submap") {
            map = gc.mMapm->mSubMap;
        } 
        else if (where == "overmap") {
            map = gc.mMapm->mOverMap;
        } 
        else if (where == "citymap") {
            map = gc.mMapm->mCityMap;
        }  

        json items = section["items"];
        for (json item : items) {
            if (item["type"].get<std::string>() == "single_ent") {
                SDL_Log("scenario: spawning single entity");
                std::string name = item["name"].get<std::string>();
                std::string demeanor = item["demeanor"].get<std::string>();
                
                int tileId = item["tile_id"].get<int>();
            
                gc.mObjm->mEntm->AllocEntityOnTable(*gc.mObjm, name, -1, -1, 0);
                Entity* ent = gc.mObjm->mEntm->mEntTable[0];
                //태도를 결정한다.
                if (demeanor == "hostile") ent->mDemeanor = Demeanor::Hostile;
                else if (demeanor == "friendly") ent->mDemeanor = Demeanor::Friendly;
                else if (demeanor == "neutral") ent->mDemeanor = Demeanor::Neutral;

                gc.mObjm->mEntm->SpawnEntityOnMap(*gc.mObjm, map, ent, tileId);
            }
        }
        mScProgress += 1; //시나리오를 자동으로 진행시키고,
        return; //업데이트 플래그가 참인 상태로 리턴하여 즉시 다음 섹션으로 이동한다.
    }
    else if (sType == "wait") {
        //일단 전투시스템부터 만들자
    }

    mIsScUpdate = false;
}

void Scenario::UpdateDialogue(GameContext& gc, json data)
{
    if (!mIsDialogueUpdate) return;

    gc.mUim->mDialogueUI->mDialogueBody->mTui->ClearTexts();
    
    json d = data[mDialogueProgress];
    std::string dType = d["type"].get<std::string>();

    //색깔 캐싱
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color white = {0xF0, 0xF0, 0xF0, 0xFF};

    if (dType == "narration") {        
        std::string text = d["text"].get<std::string>();
        gc.mUim->mDialogueUI->SetUI(text);
        mDialogueProgress += 1;

        gc.mUim->mDialogueUI->mIsRender = true;
    }
    else if (dType == "player_line") {
        TTFWord name = TTFWord("당신", white, System::sFont);
        std::string text = d["text"].get<std::string>();
        gc.mUim->mDialogueUI->SetUI(gc.mUim->mDialogueUI->mSpkrBlankImg, name, text);
        mDialogueProgress += 1;

        gc.mUim->mDialogueUI->mIsRender = true;
    }
    else if (dType == "static_line") {
        SDL_Log("static line update at dialogue");
        json speaker;
        std::string name = "";

        if (d["ent_type"].get<std::string>() == "npc") {

            json entDb = gc.mObjm->mJsm->mEntDb["items"];

            name = d["speaker"].get<std::string>();
            speaker = entDb[name];
        }
        else if (d["ent_type"].get<std::string>() == "pawn") {
            json pawnDb = gc.mObjm->mJsm->mPawnDb["items"];

            name = d["speaker"].get<std::string>();
            speaker = pawnDb[name];
        }

        std::string imgPath = speaker["img_path"].get<std::string>();
        gc.mUim->mDialogueUI->mSpeakerImg->LoadFromFile(imgPath);

        name = speaker["name"].get<std::string>();

        SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
        TTFWord entName = TTFWord(name, tc, System::sFont);

        std::string text = d["text"].get<std::string>();

        gc.mUim->mDialogueUI->SetUI(gc.mUim->mDialogueUI->mSpeakerImg, entName, text);

        mDialogueProgress += 1;

        gc.mUim->mDialogueUI->mIsRender = true;
    }
    else if (dType == "choice") {
        mIsDialogueEnd = true;
        mDialogueProgress = 0;

        gc.mUim->mDialogueUI->mIsRender = true;
    }
    else if (dType == "end") {
        mDialogueProgress = 0;
        mIsDialogueEnd = true;

        mScProgress += 1;
        mIsScUpdate = true;
        
        gc.mUim->mDialogueUI->mIsRender = false; //렌더링도 꺼야한다.
    }
    else {
        std::string text = "dialogue type not found";
        gc.mUim->mDialogueUI->SetUI(text);
        gc.mUim->mDialogueUI->mIsRender = true;
    }

    gc.mUim->mDialogueUI->mIsRenderUpdate = true;

    mIsDialogueUpdate = false; //플래그 초기화
}

void NGScenario::LoadScenarioData(GameContext& gc)
{
    JsonHelper jh;
    jh.LoadJsonFile(mData, "data/scenario/main.json");
}

void NGScenario::LoadSubMap(GameContext& gc)
{
    SDL_Log("load entities for new game scenario at submap");
    gc.mObjm->mEntm->AllocPawnOnTable(*gc.mObjm, "nameless_girl", PawnType::Unique, 0);
    SDL_Log("entities loaded");

    gc.mObjm->mEntm->SpawnEntityOnMap(*gc.mObjm, gc.mMapm->mSubMap, gc.mObjm->mEntm->mPawnTable[0], 34);
    SDL_Log("entities spawned at submap");
}

void NGScenario::LoadOverMap(GameContext& gc)
{
}

void NGScenario::LoadCityMap(GameContext& gc)
{
}

void NGScenario::Update(GameContext& gc)
{
    UpdateScenario(gc);
    UpdateDialogue(gc, mDialogueSection);
}

void DefScenario::LoadScenarioData(GameContext& gc)
{
}

void DefScenario::LoadSubMap(GameContext& gc)
{
}
void DefScenario::LoadOverMap(GameContext& gc)
{
}
void DefScenario::LoadCityMap(GameContext& gc)
{
}

void DefScenario::Update(GameContext& gc)
{
}

ScenarioManager::ScenarioManager()
{
    mCurrentSc = new DefScenario();
}

void ScenarioManager::SetCurrentScenario(Scenario *sc, GameContext& gc)
{
    DestroyCurrentScenario();
    mCurrentSc = sc;
    LoadThings(gc);
}

void ScenarioManager::DestroyCurrentScenario()
{
    if (mCurrentSc != nullptr) delete mCurrentSc;
}

void ScenarioManager::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{

}

void ScenarioManager::Update(GameContext& gc)
{
    mCurrentSc->Update(gc);
}

void ScenarioManager::LoadThings(GameContext& gc)
{
    mCurrentSc->LoadScenarioData(gc);
    mCurrentSc->LoadSubMap(gc);
    mCurrentSc->LoadOverMap(gc);
    mCurrentSc->LoadCityMap(gc);
}

void ScenarioManager::ClearThings(GameContext& gc)
{
}

