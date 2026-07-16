#include "pch.h"

#include "system.h"
#include "scenario.h"
#include "text.h"
#include "texture.h"
#include "ui.h"
#include "game_json.h"
#include "game_object.h"
#include "entity.h"

void Scenario::LoadSubMap(ObjectManager &objm)
{
}

void Scenario::LoadOverMap(ObjectManager &objm)
{
}

void Scenario::LoadCityMap(ObjectManager &objm)
{
}

void Scenario::LoadDialogue(ObjectManager &objm)
{
}

void Scenario::Update(UIManager& uim, ObjectManager &objm)
{
}

void Scenario::UpdateDialogue(UIManager &uim, ObjectManager &objm)
{

}

void NGScenario::LoadSubMap(ObjectManager& objm)
{
    SDL_Log("load entities for new game scenario at submap");
    objm.mEntm->AllocEntityOnTable(objm, "rat", 1, 1, 0);
    objm.mEntm->AllocPawnOnTable(objm, "nameless_girl", PawnType::Unique, 0);
    SDL_Log("entities loaded");

    objm.mEntm->SpawnEntityOnMap(objm, objm.mSubMap, objm.mEntm->mEntTable[0], 51);
    objm.mEntm->SpawnEntityOnMap(objm, objm.mSubMap, objm.mEntm->mPawnTable[0], 34);
    SDL_Log("entities spawned at submap");
}

void NGScenario::LoadOverMap(ObjectManager& objm)
{
}

void NGScenario::LoadCityMap(ObjectManager& objm)
{
}

void NGScenario::LoadDialogue(ObjectManager& objm)
{
    objm.mJsm->LoadJsonFile(objm.mJsm->mDialogue, "data/dialogue/intro_dialogue.json");
}

void NGScenario::Update(UIManager& uim, ObjectManager &objm)
{
    if (!mIsDialogueUpdate) return;
    SDL_Log("update by new game scenario");
    
    uim.mDialogueUI->mDialogueBody->mTui->ClearTexts();

    json dTable = objm.mJsm->mDialogue["items"];
    
    //진행도에 맞는 정보를 가져온다.
    json d = dTable["init"][mDialogueProgress]; 

    std::string dType = d["type"].get<std::string>();
    SDL_Log(dType.c_str());

    //색깔 캐싱
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color white = {0xF0, 0xF0, 0xF0, 0xFF};

    if (dType == "narration") {        
        std::string text = d["text"].get<std::string>();
        uim.mDialogueUI->SetUI(text);
        mDialogueProgress += 1;
    }
    else if (dType == "player_line") {
        TTFWord name = TTFWord("당신", white, System::sFont);
        std::string text = d["text"].get<std::string>();
        uim.mDialogueUI->SetUI(uim.mDialogueUI->mSpkrBlackImg, name, text);
        mDialogueProgress += 1;
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

        SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
        TTFWord entName = TTFWord(name, tc, System::sFont);

        std::string text = d["text"].get<std::string>();

        uim.mDialogueUI->SetUI(uim.mDialogueUI->mSpeakerImg, entName, text);

        mDialogueProgress += 1;
    }
    else if (dType == "choice") {
        mDialogueEnd = true;
        mDialogueProgress = 0;
    }
    else if (dType == "end") {
        mDialogueEnd = true;
        mDialogueProgress = 0;
    }
    else {
        std::string text = "dialogue type not found";
        uim.mDialogueUI->SetUI(text);
    }

    uim.mDialogueUI->mIsRender = true;
    uim.mDialogueUI->mIsRenderUpdate = true;

    mIsDialogueUpdate = false; //플래그 초기화
}

void DefScenario::LoadSubMap(ObjectManager& objm)
{
}
void DefScenario::LoadOverMap(ObjectManager& objm)
{
}
void DefScenario::LoadCityMap(ObjectManager& objm)
{
}

void DefScenario::LoadDialogue(ObjectManager& objm)
{
    SDL_Log("don't use this yet");
}

void DefScenario::Update(UIManager& uim, ObjectManager &objm)
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

void ScenarioManager::Update(UIManager &uim, ObjectManager &objm)
{
    mCurrentSc->Update(uim, objm);
}

void ScenarioManager::LoadThings(ObjectManager& objm)
{
    mCurrentSc->LoadSubMap(objm);
    mCurrentSc->LoadOverMap(objm);
    mCurrentSc->LoadCityMap(objm);
    mCurrentSc->LoadDialogue(objm);
}

void ScenarioManager::ClearThings(ObjectManager& objm)
{
}

