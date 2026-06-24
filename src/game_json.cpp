#include "pch.h"

#include "game_json.h"

bool JsonManager::LoadJsonFile(json& j, std::string path)
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
