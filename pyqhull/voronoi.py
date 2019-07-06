#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File            : voronoi.py
# Author          : Kai Wang <wkcosmology@gmail.com>
# Date            : 07.03.2019
# Last Modified By: Kai Wang <wkcosmology@gmail.com>

from tempfile import NamedTemporaryFile
from scipy.sparse import csr_matrix
import logging
import subprocess
import numpy as np
from itertools import product
from pyconvexhull_2d import convexhull_areas_2d_pyx


__all__ = ["PyVoronoi", "convexhull_areas_2d"]

class PyVoronoi(object):

    """Docstring for PyVoronoi. """

    def __init__(self, coordinates, add_bounding_box=False, logger=None):
        """TODO: to be defined1.

        Parameters
        ----------
        coordinates : TODO
        add_bounding_box : TODO, optional

        """
        self._input_coordinates = np.array(coordinates)
        self._add_bounding_box = add_bounding_box
        if logger is None:
            self._logger = logging.getLogger(__name__)
        else:
            self._logger = logger
        if self._add_bounding_box:
            min_edges = np.min(self._input_coordinates, axis=0)
            max_edges = np.max(self._input_coordinates, axis=0)
            width = max_edges - min_edges
            min_edges -= width
            max_edges += width
            edges = np.array((min_edges, max_edges))
            bounding_box = np.array(list(product(edges.T[0], edges.T[1], edges.T[2])))
            self._coordinates = np.concatenate((self._input_coordinates, bounding_box))
        else:
            self._coordinates = self._input_coordinates
        self._size, self._dim = self._coordinates.shape
        temp_data_file = NamedTemporaryFile("w+t")
        temp_result_file = NamedTemporaryFile("w+t")
        np.savetxt(
            temp_data_file.name,
            self._coordinates,
            delimiter="\t",
            header="%d\n%d" % (self._dim, self._size),
            comments='',
            fmt="%15.5e " * self._dim)
        cmd = "qvoronoi o < %s > %s" % (temp_data_file.name, temp_result_file.name)
        self._logger.info("Command: " + cmd)
        completed_process = subprocess.run(cmd, check=True, encoding='utf-8', shell=True)
        if completed_process.stderr is not None:
            raise Exception(completed_process)

        # read the result and store in the sparse matrix
        temp_result_file.seek(0)
        indptr = [0]
        dat = []
        for i, l in enumerate(temp_result_file):
            dat_tmp = np.fromstring(l, dtype=float, sep=" ")
            indptr.append(indptr[-1] + len(dat_tmp))
            dat.append(dat_tmp)
        indptr = np.array(indptr).astype(np.int32)
        dat = np.concatenate(dat)
        self._num_vertices = int(dat[indptr[1]: indptr[2]][0])
        self._vertices_coords = dat[indptr[2]: indptr[self._num_vertices + 2]].reshape((self._num_vertices, self._dim))
        vertices_indices = dat[indptr[self._num_vertices + 2]:].astype(np.int32)
        vertices_indptr = indptr[self._num_vertices + 2:] - indptr[self._num_vertices + 2]
        if self._add_bounding_box:
            num_add_points = int(2**self._dim)
            real_num_vertices_indices = vertices_indptr[-(num_add_points + 1)]
            self._vertices_mat = csr_matrix(
                (np.ones(real_num_vertices_indices), vertices_indices[:real_num_vertices_indices], vertices_indptr[:-num_add_points]),
                shape=(self._size - num_add_points, self._num_vertices))
        else:
            self._vertices_mat = csr_matrix(
                (np.ones(len(vertices_indices)), vertices_indices, vertices_indptr),
                shape=(self._size, self._num_vertices))


def convexhull_areas_2d(points, indptr=None):
    """calculate areas for sets of points' convex hull.

    It will first select points that can form a convex hull. Then calculate the
    area of the convex hull

    Parameters
    ----------
    points: numpy.ndarray, shape(N, 2)
        a 2d array represent the coordinates of N points
    indptr: numpy.ndarray, shape(M+1, 2), optional
        indptr[i: i+1] represent the i-th set of points that need to be calculated

    Returns
    -------
    numpy.ndarray, shape(M)
        the area of M sets of points' convex hull

    """
    if indptr is None:
        indptr = np.array([0, len(points)])
    assert points.ndim == 2
    assert indptr.ndim == 1
    assert points.shape[1] == 2
    assert points.shape[0] >= indptr[-1]
    indptr = indptr.astype(np.int32)
    points = points.astype(np.float64)
    return convexhull_areas_2d_pyx(points, indptr)
