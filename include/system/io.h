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
    void Save(Pawn* p, GameContext* gc);
}