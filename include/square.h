#pragma once

class Square {
    public:
    float GetX();
    float SetX(float x);
    float GetY();
    float SetY(float y);
    int GetW();
    int SetW(int w);
    int GetH();
    int SetH(int h);

    Square(int x, int y,int width, int height);

    void Render();
    void Render(SDL_Color lineColor, SDL_Color fillColor);
    void Render(Uint8 lineR, Uint8 lineB, Uint8 lineG, Uint8 lineA, Uint8 fillR, Uint8 fillG, Uint8 fillB, Uint8 fillA);

    private:
    float mX{0};
    float mY{0};
    int mWidth{0};
    int mHeight{0};
};