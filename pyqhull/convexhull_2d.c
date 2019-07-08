#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "convexhull_2d.h"

/******************************************************************************
* Function:         void swapp
*                   swap the content of the input pointers
* Where:
*                   void *x - TODO
*                   void *y - TODO
*                   size_t size - TODO
* Return:           void
* Error:            
*****************************************************************************/
void swapp(void *x, void *y, size_t size){
  void *tmp = malloc(size);
  memcpy(tmp, x, size);
  memcpy(x, y, size);
  memcpy(y, tmp, size);
}

/******************************************************************************
* Function:         double ccw 
* Description:      judge if the order of three points is clockwise
* Where:
*       
* Return:           positive if anti-clockwise, negative if clockwise
* Error:            
*****************************************************************************/
static double ccw(point *p1, point *p2, point *p3){
  return (p2->x - p1->x) * (p3->y - p1->y) - (p3->x - p1->x) * (p2->y - p1->y);
}

/******************************************************************************
* Function:         int compare_slope
* Description:      compare the angle between the (p_n - p_0) vector and
*                   the x-axis
* Where:
*                   void *p1 - TODO
*                   void*p2 - TODO
* Return:           1 for the angle for p1 is larger, -1 for the angle for p1 is
*                   smaller
* Error:            
*****************************************************************************/
static int compare_slope(const void *p1_in, const void *p2_in){
  point *p1 = (point *)p1_in;
  point *p2 = (point *)p2_in;
  double val = p1->x/ hypot(p1->x, p1->y) - p2->x / hypot(p2->x, p2->y);
  if (val > 0) return -1;
  else if (val < 0) return +1; 
  else return 0;
}

/******************************************************************************
* Function:         unsigned argmin_y
* Description:      find the index of point array with minimal y component
* Where:
*                   point *ps - TODO
* Return:           the index of the minimal y component
* Error:            
*****************************************************************************/
static unsigned argmin_y(point *ps, unsigned count){
  unsigned min_index = 0;
  for (unsigned i = 0; i < count; ++i) {
    if ((ps + i)->y < (ps + min_index)->y) {
      min_index = i;
    }
  }
  return min_index;
}

/******************************************************************************
* Function:         int convexhull_2d
* Description:      select the points which can form a convex hull
* Where:
*                   point *ps - TODO
*                   unsigned count - TODO
* Return:           the number of points that can form a convex hull
*                   it will also arange the convex hull points in the header
* Error:            
*****************************************************************************/
int convexhull_2d(point *ps, unsigned count){
  unsigned i;
  if (count <= 2) return count;
  unsigned min_index = argmin_y(ps, count);
  swapp(ps, ps + min_index, sizeof(point));
  for (i = 1; i < count; ++i) {
    (ps + i)->x -= ps->x;
    (ps + i)->y -= ps->y;
  }
  qsort(ps + 1, count - 1, sizeof(point), compare_slope);
  for (i = 1; i < count; ++i) {
    (ps + i)->x += ps->x;
    (ps + i)->y += ps->y;
  }

  point *chain = (point *)malloc((count + 1) * sizeof(point));
  memcpy(chain + 1, ps, count * sizeof(point));
  memcpy(chain, ps + count - 1, sizeof(point));
  int num_convex = 1;
  for (i = 2; i < count + 1; ++i) {
    while (ccw(chain + num_convex - 1, chain + num_convex, chain + i) <= 0) {
      if (num_convex > 1) {
        num_convex -= 1;
      }
      else if (i == count) {
        break;
      }
      else {
        i += 1;
      }
    }
    num_convex += 1;
    swapp(chain + num_convex, chain + i, sizeof(point));
  }
  memcpy(ps, chain + 1, count * sizeof(point));
  free(chain);
  return num_convex;
}

/******************************************************************************
* Function:         double area_triangle
* Description:      calculate the area of triangle
* Where:
*                   point *p1 - TODO
*                   point *p2 - TODO
*                   point *p3 - TODO
* Return:           
* Error:            
*****************************************************************************/
static double area_triangle(point *p1, point *p2, point *p3){
  return 0.5 * fabs((p2->x - p1->x) * (p3->y - p1->y) - (p3->x - p1->x) * (p2->y - p1->y));
}

/******************************************************************************
* Function:         double area_convexhull_2d
* Description:      calculate the area for an ordered convex hull points
* Where:
*                   point *ps - TODO
* Return:           
* Error:            
*****************************************************************************/
double area_convexhull_2d(point *ps, unsigned count){
  if (count < 3) return 0;
  double tot_area = 0;
  for (unsigned i = 1; i < count - 1; ++i) {
    tot_area += area_triangle(ps, ps + i, ps + i + 1);
  }
  return tot_area;
}

int main()
{
  point ps[8] = {{1, 1}, {5, 2}, {4, 5}, {3.5, 4.5}, {3, 4}, {2.5, 4.5}, {2, 3}, {1.5, 5}};
  int count = 8;
  unsigned i;
  int num = convexhull_2d(ps, count);
  printf("num of convex: %d\n", num);
  for (i = 0; i < (unsigned)num; ++i) {
    printf("convex point %d: (%f, %f)\n", i, (ps + i)->x, (ps + i)->y);
  }
  printf("tot area is %f\n", area_convexhull_2d(ps, num));
  return 0;
} 
