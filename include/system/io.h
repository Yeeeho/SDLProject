#pragma once

#include <string>

struct GameContext;
class Pawn;

namespace IOHelper {
    using namespace std;
    
    void MakeDir(string path);

    string Indent(int num);
    string Wrap(string message);
};

namespace CharacterIO {
    using namespace std;
    using json = nlohmann::json;

    void SavePawn(GameContext* gc, string name);
    json GetSkillData(Pawn* p);
};

namespace GameIO {
    using namespace std;

    void Save(GameContext* gc, string name);
    void MakeUserSave(string name);
};