#ifndef CONVEXHULL_H
#define CONVEXHULL_H
typedef struct {
  double x;
  double y;
} point;

int convexhull_2d(point *points, unsigned count);
double area_convexhull_2d(point *points, unsigned count);
#endif /* ifndef CONVEXHULL_H */
