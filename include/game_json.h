#pragma once

#include <json.hpp>

using json = nlohmann::json;

//Json 객체 관리하는 클래스
//TODO: 이거 삭제하고 엔티티 매니저와 장비 매니저가 데이터를 들고 있게 바꿔야됨
class JsonManager {
    public:
    JsonManager() = default;

    //데이터베이스
    json mEntDb;
    json mPawnDb;
    json mEquipmentDb;
};

class JsonHelper {
    public:
    //json 파일에서 json 객체로 파싱
    bool LoadJsonFile(json& j, std::string path);
};