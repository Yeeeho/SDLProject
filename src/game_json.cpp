#include "pch.h"

#include "game_json.h"

bool JsonHelper::LoadJsonFile(json &j, std::string path)
{
    bool success = true;

    std::ifstream file(path);
    if(!file.is_open()) {
        std::string message = path + " not opened";
        SDL_Log(message.c_str());

        success = false;
        return success;
    }

    j = json::parse(file);

    return success;
}

void JsonHelper::SDLLogErrorMsg(json &data, std::string datatype, std::string what)
{
    std::string message = "[ERROR] get " + datatype + " from json obj: unknown data param: ";
    message += what;
    message += " /type was " + GetDataType(data);
    SDL_Log(message.c_str());
}


std::string JsonHelper::GetDataType(json &data)
{
    if (!data.contains("type")) return "unknown_type";
    
    return data["type"].get<std::string>();
}

json JsonHelper::GetJson(json &data, std::string what)
{
    if (!data.contains(what)) {
        SDLLogErrorMsg(data, "json", what);
        return json();        
    }
    return data[what];
}

std::string JsonHelper::GetString(json& data, std::string what)
{
    if (!data.contains(what)) {
        SDLLogErrorMsg(data, "string", what);
        return "error";
    }
    return data[what].get<std::string>();
}

int JsonHelper::GetInt(json &data, std::string what)
{
    if (!data.contains(what)) {
        SDLLogErrorMsg(data, "int", what);
        return 0;
    }
    return data[what].get<int>();
}
