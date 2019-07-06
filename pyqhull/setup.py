from distutils.core import setup, Extension
from Cython.Build import cythonize

ext = Extension("pyconvexhull_2d",
                sources=["pyconvexhull_2d.pyx", "convexhull_2d.c"])

setup(
    name="pyconvexhull_2d",
    ext_modules=cythonize([ext])
)
