#include "pch.h"

#include "system/system.h"
#include "game_context.h"
#include "system/game_json.h"
#include "game_object.h"
#include "item/item_enum.h"
#include "item/item.h"
#include "item/item_manager.h"
#include "texture.h"
#include "combat.h"
#include "ui.h"
#include "move.h"
#include "map.h"
#include "entity.h"
#include "ai.h"
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

    namespace eu = EntityHelper;

    GameContext* gc = skm->mGc;
    LogUI* log = gc->mUim->mLogUI;
    Entity* actor = skm->mActor;
    Map* map = skm->mMap;
    std::vector<Item*>& targetItems = skm->mTargetItems;
    std::vector<Entity*>& targets = skm->mTargets;
    std::vector<int> tids = skm->mTileIds;
    json skillData = skm->mSkillData;

    int targetTileId = skm->mTileIds.back();
    MapTile* targetTile = skm->mMap->mMapTiles[targetTileId];

    SDL_Color r = {0xB0, 0x40, 0x40, 0xFF};
    SDL_Color g = {0x40, 0xB0, 0x40, 0xFF};
    SDL_Color y = {0xB0, 0xB0, 0x40, 0xFF};

    //이동 스킬일 경우
    if (skillType == "movement") {
        SkillAction::MoveAction(gc, actor, this, tids, map);
    }

    else if (skillType == "pickup") {

        if (targetItems.empty()) {
            SDL_Log("skill: target items empty");
            return;
        }

        Item* item = targetItems.back();

        Pawn* pactor = static_cast<Pawn*> (actor);
        bool itemPicked = gc->mObjm->mEntm->PickUpItemFromMap(*gc, targetTileId, item->mId, pactor);
        if (!itemPicked) return; //아이템을 줍지 못했다면 리턴한다.
        log->AddMessage(pactor->mName + "이(가) " + item->mName + "를 주웠습니다.", System::kWh);
        gc->mObjm->mItm->mIsRenderUpdate = true;
        gc->mUim->mToolTip->mIsRenderUpdate = true;
    }

    //공격 스킬일 경우
    else if (skillType == "attack") {

        //hp, ap, sp 소모 체크, 먼저 스킬 데이터에서 소모량 정보를 가져온다.
        int hpUse = SkillHelper::GetHpUse(skillData, this, actor);
        int spUse = SkillHelper::GetSpUse(skillData, this, actor);
        int apUse = SkillHelper::GetApUse(skillData, this, actor);

        //액터의 스태미너와 ap가 충분한지 확인한다.
        if (hpUse > actor->mCurHp) {
            log->AddMessage("HP가 부족합니다!", y);
            return;            
        }
        if (spUse > actor->mCurSp) {
            log->AddMessage("SP가 부족합니다!", y);
            return;            
        }
        if (apUse > actor->mCurAp) {
            log->AddMessage("AP가 부족합니다!", y);
            return;
        }

        std::string message = actor->mName + "이(가) " + skillName + "을(를) 사용합니다!";
        log->AddMessage(message, eu::GetDemeanorColor(actor));

        //액터의 스탯에서 소모량만큼 깐다.
        actor->mCurHp -= hpUse;
        actor->mCurSp -= spUse;
        actor->mCurAp -= apUse;
        gc->mUim->mBCUI->UpdateUI(actor);

        //스킬의 데미지를 구해서 타겟의 체력을 깎는다.
        int skDmg = SkillHelper::GetSkillDamage(skillData, this, actor);
        for (Entity* ent : targets) {
            ch.TakeDamage(ent, *gc, skDmg);
            if (ent->mId == actor->mId && ent->mIsPawn == actor->mIsPawn) {
                gc->mUim->mBCUI->UpdateUI(ent);
            }
        }
        gc->mUim->mToolTip->mIsRenderUpdate = true;
        skm->mTargets.clear();
    }
    else {
        SDL_Log("skill: cannot find skill type");
    }
}

SkillContext::SkillContext(Skill *skill, std::vector<Item *> &targetItems, std::vector<Entity *> &targetEnts, std::vector<int> &targetTIds)
{
    mSkill = skill;
    mTargetItems = targetItems;
    mTargetEnts = targetEnts;
    mTargetTIds = targetTIds;
}

SkillContext::SkillContext(Skill* skill, const std::vector<int> &tileIds, Map* map)
{
    mSkill = skill;
    mTargetTIds = tileIds;
    mMap = map;
}

void SkillContext::Activate(GameContext* gctx, Entity *actor)
{
    using namespace std;
    
    json sd = SkillHelper::GetSkillData(gctx->mSkm->mSkillDb, mSkill->mCode);
    
    string stype = sd["type"].get<string>();
    if (stype == "movement") SkillAction::MoveAction(gctx, actor, this);
}

SkillManager::SkillManager(GameContext& gc)
{
    mGc = &gc;
    LoadJsonData();
}

void SkillManager::LoadJsonData()
{
    
    JsonHelper::LoadJsonFile(mSkillDb, "data/skill/skill.json");
}

void SkillManager::SetSkill(Skill *skill)
{
    mSkill = skill;

    json skillTable = mGc->mSkm->mSkillDb["items"];
    mSkillData = skillTable[skill->mCode];
}

void SkillManager::SetActor(Entity *actor)
{
    mActor = actor;
}

void SkillManager::SetTargetItems(std::vector<Item*> targetItems) {
    mTargetItems = targetItems;
}

void SkillManager::SetTargetItem(Item* item) {
    mTargetItems.clear();
    mTargetItems.push_back(item);
    std::string message = "set target item: item target set to" + item->mCode;
    SDL_Log(message.c_str());
}


void SkillManager::SetTargets(std::vector<Entity *> targets)
{
    mTargets.clear();
    mTargets = targets;
}

void SkillManager::SetTarget(Entity* target)
{
    mTargets.clear();
    mTargets.push_back(target);
}

void SkillManager::SetTileIds(std::vector<int>& tileIds)
{
    std::string message = "skill manager: tile id set as: " + std::to_string(tileIds.back());
    mTileIds = tileIds;
}

void SkillManager::SetMap(Map *map)
{
    mMap = map;
}

void SkillManager::HandleEvent(SDL_Event &e, GameContext &gc)
{
    if (e.type != SDL_EVENT_KEY_DOWN) return;
    if (e.key.key == SDLK_ESCAPE) {
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
    //TODO: 스킬 발동 후 ai들의 스킬 큐를 일제히 업데이트 한다.
    EntityManager* entm = mGc->mObjm->mEntm;
    for (Entity* ent : mGc->mMapm->mCurrentMap->mNpcs) {
        Npc* npc = (Npc*) ent;
        entm->mEntAI->UpdateSkillQueue(mGc, npc);
    }
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

std::string SkillHelper::GetSkillType(GameContext *gctx, std::string code)
{
    json skilldata = GetSkillData(gctx->mSkm->mSkillDb, code);
    
    return skilldata["type"].get<std::string>();
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
            //인벤토리의 모든 무기들 데이터를 가져옴
            for (auto range = ent->mEqs.equal_range(EqType::Weapon);
                range.first != range.second; range.first++
            )
            {
                Equipment* eq = range.first->second;
                if (eq == nullptr) continue;
                int wd = range.first->second->mDamage;
                mod = ddMod["weapon_dmg_mod"].get<float>();
                dmg += (int) (wd * mod);
            }
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

Skill *SkillHelper::GetMovementSkill(Entity *ent)
{
    Skill* ret = nullptr;
    for (Skill* skill : ent->mSkills) {
        if (skill->mCode == "move") {
            ret = skill;
        }
    }

    return ret;
}

bool SkillAction::MoveAction(GameContext* gctx, Entity *actor, SkillContext *skCtx)
{
    MoveAction(gctx, actor, skCtx->mSkill, skCtx->mTargetTIds, skCtx->mMap);
    return false;
}

bool SkillAction::MoveAction(GameContext *gctx, Entity *actor, Skill *skill, std::vector<int> &tileIds, Map* map)
{
    bool success {false};

    LogUI* log = gctx->mUim->mLogUI;
    json sd = SkillHelper::GetSkillData(gctx->mSkm->mSkillDb, skill->mCode);
    //경로에 뭐가 있는지 구한다.
    for (int id : tileIds) {
        if (id == tileIds[0]) continue; //처음 아이디는 무시한다. 액터가 서있는 타일이니까..
        
        bool entOn = map->mMapTiles[id]->mIsEntOn;
        if (entOn) {
            if (actor->mIsPawn) log->AddMessage("이동 경로에 뭔가 있습니다!", System::kY);
            SDL_Log("move action: something is already on tile");
            return false;
        }
    }

    //액터의 타일당 ap소모량을 구한다이
    int apPerTile = StatHelper::GetApPerTileMove(actor); 
    float apMod = 1.f;
    if (sd["ap_per_tile"].contains("mod")){
        apMod = sd["ap_per_tile"]["mod"].get<float>();
    }
    apPerTile = (int) (apPerTile * apMod);
    
    MapTile* targetTile = map->mMapTiles[tileIds.back()];
    if (targetTile->mIsEntOn) return false; //타겟 타일에 엔티티가 있으면 리턴한다.

    MoveManager mvm = MoveManager(gctx->mUim, gctx->mObjm);
    MoveHelper mvh;

    int apCost = mvh.GetApCost(tileIds, map, apPerTile);

    if (actor->mCurAp < apCost) {
        log->AddMessage("AP가 부족합니다!", System::kY);
        SDL_Log("move action: not enough ap");
        return false;
    }        
    else {
        actor->mCurAp -= apCost;
        gctx->mUim->mBCUI->UpdateUI(actor);
    }

    //실제로 엔티티 정보를 옮기는 동작
    mvm.MoveEntityTo(map, actor, actor->mTileId, tileIds.back());
    success = true;

    return success;
}
