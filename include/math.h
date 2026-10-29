#pragma once

enum class Direction {
    East, West, North, South, NE, NW, SE, SW,
    None
};

class Direction8 {
    public:
    Direction8(Direction initDir);

    inline static std::vector<Direction> sDirections = {
        Direction::North, Direction::NE, Direction::East, Direction::SE,
        Direction::South, Direction::SW, Direction::West, Direction::NW
    };

    Direction mDir {Direction::None};

    Direction GoCW();
    Direction GoCCW();
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

