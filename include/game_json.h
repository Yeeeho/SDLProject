#pragma once

#include <json.hpp>

using json = nlohmann::json;

//Json 객체 관리하는 클래스
class JsonManager {
    public:
    JsonManager() = default;
    //json 파일에서 json 객체로 파싱
    bool LoadJsonFile(json& j, std::string path);

    json mEnemyDb;
    json mPawnDb;
    json mEquipmentDb;
};