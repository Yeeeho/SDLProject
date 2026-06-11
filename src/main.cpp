#include "pch.h"

#include "system.h"

int main() {
    System sys;
    
    if (sys.Init() == false) {
        SDL_Log("Unable to init SDL");
    }
    

    sys.Close();
}

