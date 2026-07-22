#include "pch.h"

#include "system.h"
#include "game_state.h"
#include "turn.h"
#include "map.h"
#include "city.h"
#include "scenario.h"
#include "text.h"
#include "texture.h"
#include "ui.h"
#include "game_json.h"
#include "game_object.h"
#include "entity.h"

void Scenario::LoadScenarioData(ObjectManager &objm)
{
}

void Scenario::LoadSubMap(ObjectManager &objm)
{
}

void Scenario::LoadOverMap(ObjectManager &objm)
{
}

void Scenario::LoadCityMap(ObjectManager &objm)
{
}

void Scenario::Update(UIManager &uim, ObjectManager &objm, GameStateManager& gsm)
{
}

void Scenario::UpdateScenario(UIManager& uim, ObjectManager &objm, GameStateManager& gsm)
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
        uim.mDialogueUI->mIsRenderUpdate = true;
    }
    else if (sType == "static_spawn") {
        Map* map {nullptr};
        std::string where = section["where"].get<std::string>();
        //어떤 맵에 스폰할지 결정한다.
        TurnManager* turnM;
        if (where == "submap") {
            map = objm.mSubMap;
            turnM = &gsm.mTms->mSmtm;
        } 
        else if (where == "overmap") {
            map = objm.mMap;
            turnM = &gsm.mTms->mOmtm; 
        } 
        else if (where == "citymap") {
            map = objm.mCity->mCityMap;
            turnM = &gsm.mTms->mCmtm;
        }  

        json items = section["items"];
        for (json item : items) {
            if (item["type"].get<std::string>() == "single_ent") {
                SDL_Log("scenario: spawning single entity");
                std::string name = item["name"].get<std::string>();
                std::string demeanor = item["demeanor"].get<std::string>();
                
                int tileId = item["tile_id"].get<int>();
            
                objm.mEntm->AllocEntityOnTable(objm, name, -1, -1, 0);
                Entity* ent = objm.mEntm->mEntTable[0];
                //태도를 결정한다.
                if (demeanor == "hostile") ent->mDemeanor = Demeanor::Hostile;
                else if (demeanor == "friendly") ent->mDemeanor = Demeanor::Friendly;
                else if (demeanor == "neutral") ent->mDemeanor = Demeanor::Neutral;

                objm.mEntm->SpawnEntityOnMap(objm, map, ent, tileId);
                turnM->mIsQueueUpdate = true;
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

void Scenario::UpdateDialogue(UIManager &uim, ObjectManager& objm, json data)
{
    if (!mIsDialogueUpdate) return;

    uim.mDialogueUI->mDialogueBody->mTui->ClearTexts();
    
    json d = data[mDialogueProgress];
    std::string dType = d["type"].get<std::string>();

    //색깔 캐싱
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color white = {0xF0, 0xF0, 0xF0, 0xFF};

    if (dType == "narration") {        
        std::string text = d["text"].get<std::string>();
        uim.mDialogueUI->SetUI(text);
        mDialogueProgress += 1;

        uim.mDialogueUI->mIsRender = true;
    }
    else if (dType == "player_line") {
        TTFWord name = TTFWord("당신", white, System::sFont);
        std::string text = d["text"].get<std::string>();
        uim.mDialogueUI->SetUI(uim.mDialogueUI->mSpkrBlankImg, name, text);
        mDialogueProgress += 1;

        uim.mDialogueUI->mIsRender = true;
    }
    else if (dType == "static_line") {
        SDL_Log("static line update at dialogue");
        json speaker;
        std::string name = "";

        if (d["ent_type"].get<std::string>() == "npc") {

            json entDb = objm.mJsm->mEntDb["items"];

            name = d["speaker"].get<std::string>();
            speaker = entDb[name];
        }
        else if (d["ent_type"].get<std::string>() == "pawn") {
            json pawnDb = objm.mJsm->mPawnDb["items"];

            name = d["speaker"].get<std::string>();
            speaker = pawnDb[name];
        }

        std::string imgPath = speaker["img_path"].get<std::string>();
        uim.mDialogueUI->mSpeakerImg->LoadFromFile(imgPath);

        name = speaker["name"].get<std::string>();

        SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
        TTFWord entName = TTFWord(name, tc, System::sFont);

        std::string text = d["text"].get<std::string>();

        uim.mDialogueUI->SetUI(uim.mDialogueUI->mSpeakerImg, entName, text);

        mDialogueProgress += 1;

        uim.mDialogueUI->mIsRender = true;
    }
    else if (dType == "choice") {
        mIsDialogueEnd = true;
        mDialogueProgress = 0;

        uim.mDialogueUI->mIsRender = true;
    }
    else if (dType == "end") {
        mDialogueProgress = 0;
        mIsDialogueEnd = true;

        mScProgress += 1;
        mIsScUpdate = true;
        
        uim.mDialogueUI->mIsRender = false; //렌더링도 꺼야한다.
    }
    else {
        std::string text = "dialogue type not found";
        uim.mDialogueUI->SetUI(text);
        uim.mDialogueUI->mIsRender = true;
    }

    uim.mDialogueUI->mIsRenderUpdate = true;

    mIsDialogueUpdate = false; //플래그 초기화
}

void NGScenario::LoadScenarioData(ObjectManager &objm)
{
    JsonHelper jh;
    jh.LoadJsonFile(mData, "data/scenario/main.json");
}

void NGScenario::LoadSubMap(ObjectManager &objm)
{
    SDL_Log("load entities for new game scenario at submap");
    objm.mEntm->AllocPawnOnTable(objm, "nameless_girl", PawnType::Unique, 0);
    SDL_Log("entities loaded");

    objm.mEntm->SpawnEntityOnMap(objm, objm.mSubMap, objm.mEntm->mPawnTable[0], 34);
    SDL_Log("entities spawned at submap");
}

void NGScenario::LoadOverMap(ObjectManager& objm)
{
}

void NGScenario::LoadCityMap(ObjectManager& objm)
{
}

void NGScenario::Update(UIManager& uim, ObjectManager &objm, GameStateManager& gsm)
{
    UpdateScenario(uim, objm, gsm);
    UpdateDialogue(uim, objm, mDialogueSection);
}

void DefScenario::LoadScenarioData(ObjectManager &objm)
{
}

void DefScenario::LoadSubMap(ObjectManager &objm)
{
}
void DefScenario::LoadOverMap(ObjectManager& objm)
{
}
void DefScenario::LoadCityMap(ObjectManager& objm)
{
}

void DefScenario::Update(UIManager& uim, ObjectManager &objm, GameStateManager& gsm)
{
}

ScenarioManager::ScenarioManager()
{
    mCurrentSc = new DefScenario();
}

void ScenarioManager::SetCurrentScenario(Scenario *sc, ObjectManager& objm)
{
    DestroyCurrentScenario();
    mCurrentSc = sc;
    LoadThings(objm);
}

void ScenarioManager::DestroyCurrentScenario()
{
    if (mCurrentSc != nullptr) delete mCurrentSc;
}

void ScenarioManager::HandleEvent(SDL_Event &e, UIManager &uim, ObjectManager &objm, float mouseX, float mouseY)
{

}

void ScenarioManager::Update(UIManager &uim, ObjectManager &objm, GameStateManager& gsm)
{
    mCurrentSc->Update(uim, objm, gsm);
}

void ScenarioManager::LoadThings(ObjectManager& objm)
{
    mCurrentSc->LoadScenarioData(objm);
    mCurrentSc->LoadSubMap(objm);
    mCurrentSc->LoadOverMap(objm);
    mCurrentSc->LoadCityMap(objm);
}

void ScenarioManager::ClearThings(ObjectManager& objm)
{
}

