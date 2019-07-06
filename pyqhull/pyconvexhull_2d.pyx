from libc.stdlib cimport malloc, free
import numpy as np
cimport numpy as np

cdef extern from "convexhull_2d.h":
    ctypedef struct point:
        double x;
        double y;

    int convexhull_2d(point[] ps, unsigned count);
    double area_convexhull_2d(point[] ps, unsigned count);

def pyconvexhull_2d(np.ndarray[np.double_t, ndim=1] xs, np.ndarray[np.double_t, ndim=1] ys):

    num_points = xs.shape[0]

    cdef:
        point *ps = <point *> malloc(num_points * sizeof(point))
        int i
        unsigned num_convex
        double area

    for i in range(num_points):
        ps[i].x = xs[i]
        ps[i].y = ys[i]
    num_convex = convexhull_2d(ps, num_points)
    area = area_convexhull_2d(ps, num_convex)
    for i in range(num_convex):
        xs[i] = ps[i].x
        ys[i] = ps[i].y
    return float(area), np.column_stack([xs[:num_convex], ys[:num_convex]])


def convexhull_areas_2d_pyx(np.ndarray[np.double_t, ndim=2] points, np.ndarray[int, ndim=1] indptr):
    num_hull = indptr.shape[0] - 1
    num_all_points = points.shape[0]

    cdef:
        point *ps = <point *> malloc(num_all_points * sizeof(point))
        int i
        unsigned num_convex
        unsigned num_points
        unsigned start_p
        double area
        np.ndarray[double, ndim=1] areas = np.empty(num_hull)

    for i in range(num_all_points):
        ps[i].x = points[i][0]
        ps[i].y = points[i][1]

    for i in range(num_hull):
        start_p = indptr[i]
        num_points = indptr[i + 1] - indptr[i]
        num_convex = convexhull_2d(ps + start_p, num_points)
        area = area_convexhull_2d(ps + start_p, num_convex)
        areas[i] = area
    return areas
