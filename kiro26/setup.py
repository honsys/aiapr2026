from setuptools import setup
from Cython.Build import cythonize

setup(
    ext_modules=cythonize("geoip_last.py", compiler_directives={"language_level": "3"})
)
