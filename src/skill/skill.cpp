#include "pch.h"

#include "game_context.h"
#include "game_json.h"
#include "game_object.h"
#include "item/item_enum.h"
#include "item/item.h"
#include "texture.h"
#include "combat.h"
#include "ui.h"
#include "move.h"
#include "map.h"
#include "entity.h"
#include "skill/skill.h"

Skill::Skill(std::string code, std::string name)
{
    mCode = code;
    mName = name;
}

void Skill::Activate(SkillManager* skm)
{
    std::string skillName = skm->mSkillData["name"].get<std::string>();
    std::string skillType = skm->mSkillData["type"].get<std::string>();

    CombatHelper ch;
    SkillHelper skh;
    StatHelper sh;

    GameContext* gc = skm->mGc;
    Entity* actor = skm->mActor;
    Map* map = skm->mMap;
    std::vector<Entity*> targets = skm->mTargets;
    std::vector<int> tids = skm->mTileIds;
    json skillData = skm->mSkillData;

    int targetTileId = skm->mTileIds.back();
    MapTile* targetTile = skm->mMap->mMapTiles[targetTileId];

    //아직 스태미너 계산은 안넣었다.
    //이동 스킬일 경우
    if (skillType == "movement") {
        //TODO:나중에 스킬헬퍼 만들어서 함수로 래핑해라

        //경로에 뭐가 있는지 구한다.
        for (int id : tids) {
            if (id == tids[0]) continue; //처음 아이디는 무시한다. 액터가 서있는 타일이니까..
            bool entOn = map->mMapTiles[id]->mIsEntOn;
            if (entOn) {
                SDL_Log("경로에 뭔가 있습니다!");
                return;
            }
        }

        //액터의 타일당 ap소모량을 구한다이
        int apPerTile = sh.GetApPerTileMove(skm->mActor); 
        float apMod = 1.f;
        if (skm->mSkillData["ap_per_tile"].contains("mod")){
            apMod = skm->mSkillData["ap_per_tile"]["mod"].get<float>();
        }
        apPerTile = (int) (apPerTile * apMod);
        
        MapTile* targetTile = skm->mMap->mMapTiles[targetTileId];
        if (targetTile->mIsEntOn) return; //타겟 타일에 엔티티가 있으면 리턴한다.

        MoveManager mvm = MoveManager(gc->mUim, gc->mObjm);
        MoveHelper mvh;

        //대각선 이동 개수와 수직 이동 개수를 구한다.
        int diaMoves = mvh.GetDiagonalMoves(tids, map);
        int straightMoves = tids.size() - 1 - diaMoves;

        int apCost = apPerTile * straightMoves + apPerTile * diaMoves * 1.5;
        if (actor->mCurAp < apCost) {
            SDL_Log("ap가 부족합니다!");
            return;
        }        
        else {
            actor->mCurAp -= apCost;
            gc->mUim->mBCUI->UpdateUI(actor);
        }

        //실제로 엔티티 정보를 옮기는 동작
        mvm.MoveEntityTo(map, actor, actor->mTileId, targetTileId);
        std::string message = skm->mActor->mName + " 이 이동합니다!";
        SDL_Log(message.c_str());
    }

    //공격 스킬일 경우
    else if (skillType == "attack") {

        //hp, ap, sp 소모 체크, 먼저 스킬 데이터에서 소모량 정보를 가져온다.
        int hpUse = skh.GetHpUse(skillData, this, actor);
        int spUse = skh.GetSpUse(skillData, this, actor);
        int apUse = skh.GetApUse(skillData, this, actor);

        //액터의 스태미너와 ap가 충분한지 확인한다.
        if (hpUse > actor->mCurHp) {
            SDL_Log("hp가 부족합니다!");
            return;            
        }
        if (spUse > actor->mCurSp) {
            SDL_Log("sp가 부족합니다!");
            return;            
        }
        if (apUse > actor->mCurAp) {
            SDL_Log("ap가 부족합니다!");
            return;
        }

        std::string message = actor->mName + "이(가) " + skillName + "을(를) 사용합니다!";
        SDL_Log(message.c_str());

        //액터의 스탯에서 소모량만큼 깐다.
        actor->mCurHp -= hpUse;
        actor->mCurSp -= spUse;
        actor->mCurAp -= apUse;
        gc->mUim->mBCUI->UpdateUI(actor);

        //스킬의 데미지를 구해서 타겟의 체력을 깎는다.
        int skDmg = skh.GetSkillDamage(skillData, this, actor);
        for (Entity* ent : targets) {
            ch.TakeDamage(ent, *gc, skDmg);
            if (ent->mId == actor->mId && ent->mIsPawn == actor->mIsPawn) {
                gc->mUim->mBCUI->UpdateUI(ent);
            }
            //래핑?
            gc->mUim->UpdateMapToolTip(map, ent, targetTileId);
        }

    }
    else {
        SDL_Log("skill: cannot find skill type");
    }
}


SkillManager::SkillManager(GameContext& gc)
{
    mGc = &gc;
    LoadJsonData();
}

void SkillManager::LoadJsonData()
{
    JsonHelper jh;
    jh.LoadJsonFile(mSkillDb, "data/skill/skill.json");
}

void SkillManager::SetSkill(Skill * skill)
{
    mSkill = skill;
}

void SkillManager::SetSkillData(json skilldata)
{
    mSkillData = skilldata;
}

void SkillManager::SetActor(Entity *actor)
{
    mActor = actor;
}

void SkillManager::SetTargets(std::vector<Entity *> targets)
{
    mTargets.clear();
    mTargets = targets;
}

void SkillManager::SetTileIds(std::vector<int> tileIds)
{
    mTileIds = tileIds;
}

void SkillManager::SetMap(Map *map)
{
    mMap = map;
}

void SkillManager::HandleEvent(SDL_Event &e, GameContext &gc)
{
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
    if (e.button.button == SDL_BUTTON_RIGHT) {
        //오른쪽 마우스 버튼 클릭시
        mIsSkillReady = false; //스킬 준비 해제
    }
}

void SkillManager::ActivateSkill()
{
    if (!mActor) {
        SDL_Log("skill manager: actor is null!");
        return;
    }

    json skillDb = mSkillDb["items"];
    if (!skillDb.contains(mSkill->mCode)) {
        SDL_Log("skill manager: cannot find skill code in skill database!");
        return;
    } 
    json skillData = skillDb[mSkill->mCode];

    mSkill->Activate(this);
}

//나중에 스킬 객체에 있는 추가 보정치도 포함할 수 있다.
int SkillHelper::GetSkillRange(json skillData, Skill *skill)
{
    int range = skillData["range"].get<int>();
    return range;
}
json SkillHelper::GetSkillData(const json &skillDb, std::string code)
{
    json ret;
    if (skillDb["items"].contains(code)) {
        ret = skillDb["items"][code];
    }
    else {
        SDL_Log("getskilldata: cannot find code in skill db!");
    }
    return ret;
}
std::string SkillHelper::GetSkillTargetType(json skillData, Skill *skill)
{
    std::string target = skillData["target"].get<std::string>();
    return target;
}

std::string SkillHelper::GetSkillName(json skillData, Skill *skill)
{
    std::string name = skillData["name"].get<std::string>();
    return name;
}
int SkillHelper::GetSkillTargetNum(json skillData, Skill *skill)
{
    std::string targetType = GetSkillTargetType(skillData, skill);
    if (targetType == "single") return 1;
    else {
        SDL_Log("아직 안만들었슴미다 ㅋㅋ");
        return 1;
    }
}

int SkillHelper::GetSkillDamage(json skillData, Skill *skill, Entity *ent)
{
    int dmg = 0;
    float mod = 0.f;
    int stat = 0;
    int flat = 0;

    if (!skillData.contains("dmg")) {
        SDL_Log("skill helper: there is no dmg in skilldata.");
        return 0;
    }

    json dd = skillData["dmg"];
    if (dd.contains("mod")) {
        json ddMod = dd["mod"];
        if (ddMod.contains("weapon_dmg_mod")) {
            int wd = ent->mEqs[EqType::Weapon]->mDamage;
            mod = ddMod["weapon_dmg_mod"].get<float>();
            dmg += (int) (wd * mod);
        }
        if (ddMod.contains("str_mod")) {
            mod = ddMod["str_mod"].get<float>();
            stat = ent->mStr;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("end_mod")) {
            mod = ddMod["end_mod"].get<float>();
            stat = ent->mEnd;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("dex_mod")) {
            mod = ddMod["dex_mod"].get<float>();
            stat = ent->mDex;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("agi_mod")) {
            mod = ddMod["agi_mod"].get<float>();
            stat = ent->mAgi;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("wil_mod")) {
            mod = ddMod["wil_mod"].get<float>();
            stat = ent->mWil;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("int_mod")) {
            mod = ddMod["int_mod"].get<float>();
            stat = ent->mInt;
            dmg += (int) (stat * mod);
        }
        if (ddMod.contains("spd_mod")) {
            mod = ddMod["spd_mod"].get<float>();
            stat = ent->mSpd;
            dmg += (int) (stat * mod);
        }
    }
    if (dd.contains("flat")) {
        json ddFlat = dd["flat"];
        SDL_Log("플랫 데미지는 아직 구현 안했다");
    }

    return dmg;
}

int SkillHelper::GetHpUse(json skillData, Skill *skill, Entity *ent)
{
    int hpUse = 0;

    if (skillData.contains("hp_use")){
        json hpu = skillData["hp_use"];
        if (hpu.contains("flat")) {
            hpUse += hpu["flat"].get<int>();
        }
    }

    return hpUse;
}

int SkillHelper::GetSpUse(json skillData, Skill *skill, Entity *ent)
{
    int spUse = 0;

    if (skillData.contains("sp_use")){
        json spu = skillData["sp_use"];
        if (spu.contains("flat")) {
            spUse += spu["flat"].get<int>();
        }
    }

    return spUse;
}

int SkillHelper::GetApUse(json skillData, Skill *skill, Entity *ent)
{
    int apUse = 0;

    if (skillData.contains("ap_use")){
        json apu = skillData["ap_use"];
        if (apu.contains("flat")) {
            apUse += apu["flat"].get<int>();
        }
    }

    return apUse;
}
