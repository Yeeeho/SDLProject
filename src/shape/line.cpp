#include "pch.h"

#include "line.h"

Line::Line(Point *p1, Point *p2)
{
    mP1 = p1; mP2 = p2;
}

Line::~Line()
{
    delete mP1;
    delete mP2;
}
