#pragma once

class Square {
    public:
    int GetX();
    int GetY();

    Square(int x, int y,int width, int height);

    void Render();

    private:
    int mX{0};
    int mY{0};
    int mWidth{0};
    int mHeight{0};
};