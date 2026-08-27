@echo initiate compiling

@echo making pch
g++ -x c++-header^
 -I./include -I./lib -I./lib/SDL3/include -I./lib/SDL3_ttf/include^
 -I./lib/SDL3_image/include -I./lib/SDL3_mixer/include^
 include/pch.h -o include/pch.h.gch

@echo compiling source files
::소스파일 폴더에 하위 디렉토리가 있다면 여기에 추가해야 한다.
g++ src/*.cpp src/item/*.cpp src/skill/*.cpp src/system/*.cpp^
 -I./include -I./lib -I./lib/SDL3/include -I./lib/SDL3_ttf/include^
 -I./lib/SDL3_image/include -I./lib/SDL3_mixer/include^

 -L./lib/SDL3/lib -L./lib/SDL3_ttf/lib -L./lib/SDL3_image/lib -L./lib/SDL3_mixer/lib^
 -lSDL3 -lSDL3_ttf -lSDL3_image -lSDL3_mixer^
 -include include/pch.h -o geimu.exe

@echo deleting pch file
erase ".\include\pch.h.gch"

@echo completed compiling