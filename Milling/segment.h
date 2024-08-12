
#ifndef SEGMENT_H
#define SEGMENT_H

#include <iostream>
#include <cmath>
#include <QQueue>

struct Coordinates
{
    Coordinates(){}
    Coordinates(double x, double y, double z): x_coordinate(x), y_coordinate(y), z_coordinate(z) {}
    double x_coordinate = 0;
    double y_coordinate = 0;
    double z_coordinate = 0;
};

bool operator<(const Coordinates& center_point, const Coordinates& begin_point);

struct Segment
{
    Segment();
    Segment(const Coordinates&, const Coordinates&);
    Segment(const Coordinates&, const Coordinates&, const Coordinates&);
    Segment(const Coordinates&, const Coordinates&, const Coordinates&, double );
    Coordinates begin;
    Coordinates end;
    Coordinates center;
    Coordinates middle;
    double radius;
};

bool CheckingUnderLine(const Segment& segment);
bool CheckingSegment(const Segment& check_segment);
Coordinates MakeMiddleRadiusRight(const Segment& segment);
Coordinates MakeMiddleRadiusLeft(const Segment& segment);
void CrushingRecursion(const Segment segment, QQueue<Coordinates>&);
void CrushingSegmentRight(const Segment segment, QQueue<Coordinates>&);
void CrushingSegmentLeft(const Segment segment, QQueue<Coordinates>&);



#endif // SEGMENT_H
