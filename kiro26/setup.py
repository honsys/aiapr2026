import sys
from setuptools import setup, Extension
from Cython.Build import cythonize

# Suppress warnings in Cython-generated C code (not our code, so silence them)
_suppress = [
    "-Wno-unused-function",
    "-Wno-unreachable-code",
    "-Wno-unused-variable",
    "-Wno-deprecated-declarations",
]

# clang (macOS) needs this extra flag
if sys.platform == "darwin":
    _suppress += ["-Wno-unreachable-code-fallthrough", "-Wno-sometimes-uninitialized"]

ext = Extension(
    "geoip_last",
    sources=["geoip_last.py"],
    extra_compile_args=_suppress,
)

setup(
    ext_modules=cythonize(
        ext,
        compiler_directives={
            "language_level": "3",
            "boundscheck": False,
            "wraparound": False,
        },
        quiet=True,
    )
)
