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

Skill::Skill(std::string code)
{
    mCode = code;
}

void Skill::Activate(
    GameContext& gc,
    json skillData, Entity *actor, std::vector<Entity *> targets,
    Map *map, int targetTileId
)
{
    std::string skillName = skillData["name"].get<std::string>();
    std::string skillType = skillData["type"].get<std::string>();

    CombatHelper ch;
    SkillHelper skh;
    StatHelper sh;
    //아직 스태미너 계산은 안넣었다.
    if (skillType == "movement") {
        std::string message = actor->mName + " 이 이동합니다!";
        SDL_Log(message.c_str());
        //나중에 스킬헬퍼 만들어서 함수로 래핑해라

        int apPerTile = sh.GetApPerTileMove(actor); //액터의 타일당 ap소모량을 구한다이
        float apMod = 0.f;
        if (skillData["ap_per_tile"].contains("mod")){
            apMod = skillData["ap_per_tile"]["mod"].get<float>();
        }
        apPerTile = (int) (apPerTile * apMod);
        //이부분에서 엔티티의 실제 ap에서 차감시켜야한다.

        MoveManager mvm = MoveManager(gc.mUim, gc.mObjm);
        mvm.MoveEntityTo(map, actor, actor->mTileId, targetTileId);
    }
    else if (skillType == "attack") {
        if (targets.empty()) {
            SDL_Log("스킬을 사용할 대상이 없읍니다.");
            return;
        }
        std::string message = actor->mName + " 이 " + skillName + " 을 사용합니다!";
        SDL_Log(message.c_str());
        //데미지를 구해서 체력을 깎는다.
        //하지만 나중에는 takedamage 함수를 만들어서 데미지 계산 동작을 제대로 구현할것
        int skDmg = skh.GetSkillDamage(skillData, this, actor);
        for (Entity* ent : targets) {
            ch.TakeDamage(ent, gc, skDmg);
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

void SkillManager::SetMap(Map *map)
{
    mMap = map;
}

void SkillManager::SetTargetTileId(int tid)
{
    mTid = tid;
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

    mSkill->Activate(*mGc, skillData, mActor, mTargets, mMap, mTid);
}

//나중에 스킬 객체에 있는 추가 보정치도 포함할 수 있다.
int SkillHelper::GetSkillRange(json skillData, Skill *skill)
{
    int range = skillData["range"].get<int>();
    return range;
}
std::string SkillHelper::GetSkillTargetType(json skillData, Skill *skill)
{
    std::string target = skillData["target"].get<std::string>();
    return target;
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
