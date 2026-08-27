#include "pch.h"

#include "system/io.h"

void IOHelper::MakeDir(string path)
{
    ofstream out;
    out.open(path);

    out.close();
}

std::string IOHelper::Indent(int num)
{
    using namespace std;

    string ret = "";
    for (int i = 0; i < num; i++) {
        ret += " ";
    }

    return ret;
}

std::string IOHelper::Wrap(std::string message)
{
    return "\"" + message + "\"";
}

void CharacterIO::Save(Pawn *p, GameContext *gc)
{
    using namespace std;

    ofstream out;
}
