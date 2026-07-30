#pragma once

#include <string>
#include "item_enum.h"

//전방 선언
class ObjectManager;

//아이템 최상위 인터페이스 클래스
class Item {
    public:
    Item() = default;
    Item(std::string name);

    //아이템 속성
    std::string mCode = ""; //데이터베이스 식별용 코드. 런타임에서 바꾸면 안됨
    std::string mName = ""; //게임 내에서 실제로 보일 이름
    int mValue = 0; //가치, 실제 계산 방식은 여러 변수들에 의해 동적으로 변한다.
};

//장비 클래스
class Equipment : public Item {
    public:
    Equipment(const ObjectManager& objm, std::string code);

    //장비 속성
    
    EqType mEqType {0};
    //기본 공격력 등은 데이터베이스를 참고하도록 한다.
    //아래에 정의된 변수들은 현재 상태에 관한 것이다.
    float mWeight {0.f};

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
