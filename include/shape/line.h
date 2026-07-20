#pragma once

struct Point;

class Line {
    public:
    Line(Point* p1, Point* p2);
    ~Line();

    Point* mP1 {nullptr};
    Point* mP2 {nullptr};
};