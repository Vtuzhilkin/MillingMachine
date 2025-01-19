#ifndef ARC_H
#define ARC_H
#include <QVector>

struct Point {
    float x, y, z;
    Point() : x(0), y(0), z(0) {}
    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Arc
{
public:
    Arc(const Point& start, const Point& end, const Point& center, float maxDist = 0.25);

    QVector<Point> getArcPoints() const;

private:
    Point start, end, center;
    double maxDist;
    QVector<Point> arcPoints;

    // Метод для вычисления точек на дуге
    void calculateArcPoints();
    // Функция для вычисления длины вектора
    float vectorLength(const Point& p);

    // Скалярное произведение двух векторов
    float dotProduct(const Point& a, const Point& b);

    // Векторное произведение двух векторов
    Point crossProduct(const Point& a, const Point& b);

    // Нормализация вектора
    Point normalize(const Point& p);
};

#endif // ARC_H
