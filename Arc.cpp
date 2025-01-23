#include "Arc.h"

Arc::Arc(const Point &start, const Point &end, const Point &center, float maxDist, bool clockwise)
    : start(start), end(end), center(center), maxDist(maxDist), clockwise(clockwise) {
    calculateArcPoints();
}

QVector<Point> Arc::getArcPoints() const {
    return arcPoints;
}

void Arc::calculateArcPoints() {
    // Векторы от центра к начальной и конечной точкам дуги
    Point CA = {start.x - center.x, start.y - center.y, start.z - center.z};
    Point CB = {end.x - center.x, end.y - center.y, end.z - center.z};

    // Длина вектора CA (радиус дуги)
    float radius = vectorLength(CA);

    Point perpendicular = crossProduct(CA, CB);

    // Угол дуги между CA и CB
    float cosTheta = dotProduct(CA, CB) / (vectorLength(CA) * vectorLength(CB));
    float theta = std::acos(cosTheta);
    if((perpendicular.z > 0 && clockwise) || (perpendicular.z < 0 && !clockwise)){
        theta = 2*std::acos(-1) - theta;
    }

    // Количество точек, которое нужно для того, чтобы расстояние между точками было <= maxDist
    int N = std::ceil((theta * radius) / maxDist);

    // Ортогональные единичные векторы в плоскости дуги
    Point u = normalize(CA);
    Point v = normalize(crossProduct(crossProduct(CA, CB), CA));
    if((perpendicular.z > 0 && clockwise) || (perpendicular.z < 0 && !clockwise)){
        v = {-v.x, -v.y, -v.z};
    }

    // Угол между точками на дуге
    float deltaTheta = theta / N;

    // Вычисляем координаты промежуточных точек
    arcPoints.clear();  // Очистим старые точки
    for (int i = 0; i <= N; ++i) {
        float angle = i * deltaTheta;
        Point point = {
            center.x + radius * (std::cos(angle) * u.x + std::sin(angle) * v.x),
            center.y + radius * (std::cos(angle) * u.y + std::sin(angle) * v.y),
            center.z + radius * (std::cos(angle) * u.z + std::sin(angle) * v.z)
        };
        arcPoints.push_back(point);
    }
}

float Arc::vectorLength(const Point &p) {
    return std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

float Arc::dotProduct(const Point &a, const Point &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point Arc::crossProduct(const Point &a, const Point &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

Point Arc::normalize(const Point &p) {
    float length = vectorLength(p);
    return {p.x / length, p.y / length, p.z / length};
}
