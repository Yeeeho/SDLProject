#pragma once

#include <string>

enum class ItemType {
    Consumable, Artifact, Equipment
};

//전방 선언
class ObjectManager;
//아이템 최상위 인터페이스 클래스
class Item {
    public:
    Item() = default;
    Item(std::string name);

    //아이템 속성
    std::string mName = ""; //이름
    int mValue = 0; //가치, 실제 계산 방식은 여러 변수들에 의해 동적으로 변한다.
};

enum class EqType {
    Head, Torso, Leg, Hand, Foot, Weapon, Offhand
};

//장비 클래스
class Equipment : public Item {
    public:
    Equipment(const ObjectManager& objm, std::string name);

    //장비 속성
    //기본 공격력 등은 데이터베이스를 참고하도록 한다.
    //아래에 정의된 변수들은 현재 상태에 관한 것이다.
    int mDamage {0};
    int mArmor {0};
    //깡스탯 증가
    int mStrFlat {0};
    int mEndFlat {0};
    int mDexFlat {0};
    int mAgiFlat {0};
    int mWilFlat {0};
    int mIntFlat {0};
    int mSpdFlat {0};

    
};