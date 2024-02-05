
#include "segment.h"

Segment::Segment(const Coordinates& bg, const Coordinates& en): begin(bg), end(en){
    middle.x_coordinate = begin.x_coordinate + (end.x_coordinate-begin.x_coordinate)/2;
    middle.y_coordinate = begin.y_coordinate + (end.y_coordinate-begin.y_coordinate)/2;
    middle.z_coordinate = begin.z_coordinate;
};

Segment::Segment(const Coordinates& bg, const Coordinates& en, const Coordinates& ctr): Segment(bg, en) {
    center.x_coordinate = begin.x_coordinate + ctr.x_coordinate;
    center.y_coordinate = begin.y_coordinate + ctr.y_coordinate;
    center.z_coordinate = begin.z_coordinate;
    radius = sqrt(pow(begin.x_coordinate-center.x_coordinate,2) + pow(begin.y_coordinate-center.y_coordinate,2));
};

Segment::Segment(const Coordinates& bg, const Coordinates& en, const Coordinates& ctr, double rad): Segment(bg, en) {
    center = ctr;
    radius = rad;
};

bool CheckingUnderLine(const Segment& segment){
    double coef_k = (segment.begin.y_coordinate-segment.center.y_coordinate)/(segment.begin.x_coordinate-segment.center.x_coordinate);
    double coef_b = segment.begin.y_coordinate - coef_k*segment.begin.x_coordinate;
    if(segment.end.y_coordinate < (coef_k*segment.end.x_coordinate + coef_b)){
        return true;
    }else{
        return false;
    }
}

bool operator< (const Coordinates& center_Coordinates, const Coordinates& begin_Coordinates){
    if(center_Coordinates.x_coordinate <= begin_Coordinates.x_coordinate){
        return true;
    }else if(center_Coordinates.x_coordinate > begin_Coordinates.x_coordinate){
        return false;
    }
    return true;
}

bool CheckingSegment(const Segment& check_segment){
    if(!(CheckingUnderLine(check_segment) ^ (check_segment.center < check_segment.begin))){
        return true;
    }else{
        return false;
    }

}

Coordinates MakeMiddleRadiusRight(const Segment& segment){
    Coordinates middle_radius;
    double alfa = atan(fabs((segment.middle.y_coordinate-segment.center.y_coordinate)/(segment.middle.x_coordinate-segment.center.x_coordinate)));
    if(segment.middle.x_coordinate > segment.center.x_coordinate){
        middle_radius.x_coordinate = segment.center.x_coordinate + segment.radius*cos(alfa);
    }else{
        middle_radius.x_coordinate = segment.center.x_coordinate - segment.radius*cos(alfa);
    }
    if(segment.middle.y_coordinate > segment.center.y_coordinate){
        middle_radius.y_coordinate = segment.center.y_coordinate + segment.radius*sin(alfa);
    }else{
        middle_radius.y_coordinate = segment.center.y_coordinate - segment.radius*sin(alfa);
    }
    middle_radius.z_coordinate = segment.center.z_coordinate;
    return middle_radius;
}

Coordinates MakeMiddleRadiusLeft(const Segment& segment){
    Coordinates middle_radius;
    double alfa = atan(fabs((segment.middle.y_coordinate-segment.center.y_coordinate)/(segment.middle.x_coordinate-segment.center.x_coordinate)));
    if(segment.middle.x_coordinate > segment.center.x_coordinate){
        middle_radius.x_coordinate = segment.center.x_coordinate - segment.radius*cos(alfa);
    }else{
        middle_radius.x_coordinate = segment.center.x_coordinate + segment.radius*cos(alfa);
    }
    if(segment.middle.y_coordinate > segment.center.y_coordinate){
        middle_radius.y_coordinate = segment.center.y_coordinate - segment.radius*sin(alfa);
    }else{
        middle_radius.y_coordinate = segment.center.y_coordinate + segment.radius*sin(alfa);
    }
    middle_radius.z_coordinate = segment.center.z_coordinate;
    return middle_radius;
}

void CrushingRecursion(const Segment segment, QQueue<Coordinates>& coordinates){
    Coordinates middle_radius = MakeMiddleRadiusRight(segment);
    QQueue<Coordinates>::const_iterator last_Coordinates = --coordinates.cend();
    double length_to_begin = sqrt(pow(middle_radius.x_coordinate-last_Coordinates->x_coordinate,2)+pow(middle_radius.y_coordinate-last_Coordinates->y_coordinate,2));
    if(length_to_begin < 0.25){
        coordinates.push_back(middle_radius);
        coordinates.push_back(segment.end);
        return;
    }else{
        Segment segment_first(coordinates.back(), middle_radius, segment.center, segment.radius);
        CrushingRecursion(segment_first, coordinates);
        Segment segment_second(coordinates.back(), segment.end, segment.center, segment.radius);
        CrushingRecursion(segment_second, coordinates);
    }
}
void CrushingSegmentRight(const Segment segment, QQueue<Coordinates>& coordinates){
    coordinates.push_back(segment.begin);
    if(CheckingSegment(segment)){
        CrushingRecursion(segment, coordinates);
    }else{
        Coordinates middle_radius = MakeMiddleRadiusLeft(segment);
        Segment first_segment = Segment(segment.begin, middle_radius, segment.center, segment.radius);
        CrushingRecursion(first_segment, coordinates);
        Segment second_segment = Segment(middle_radius, segment.end, segment.center, segment.radius);
        CrushingRecursion(second_segment, coordinates);
    }
}

void CrushingSegmentLeft(const Segment segment, QQueue<Coordinates>& coordinates){
    coordinates.push_back(segment.begin);
    if(!CheckingSegment(segment)){
        CrushingRecursion(segment, coordinates);
    }else{
        Coordinates middle_radius = MakeMiddleRadiusLeft(segment);
        Segment first_segment = Segment(segment.begin, middle_radius, segment.center, segment.radius);
        CrushingRecursion(first_segment, coordinates);
        Segment second_segment = Segment(middle_radius, segment.end, segment.center, segment.radius);
        CrushingRecursion(second_segment, coordinates);
    }
}



