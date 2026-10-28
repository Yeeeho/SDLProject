#pragma once

enum class Direction {
    East, West, North, South, NE, NW, SE, SW,
    None
};

struct Vector2 {
    public:
    int x {0};
    int y {0};
};

class Math {
    public:
    bool IsPointInSquare(float pointX, float pointY, float mX, float mY, float mW, float mH);
    Direction GetDirection8(int vectorX, int vectorY);
};

