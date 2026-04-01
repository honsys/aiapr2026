import sys
from setuptools import setup, Extension
from Cython.Build import cythonize

_suppress = [
  "-Wno-unused-function",
  "-Wno-unreachable-code",
  "-Wno-unused-variable",
  "-Wno-deprecated-declarations",
]

if sys.platform == "darwin":
  _suppress += ["-Wno-unreachable-code-fallthrough", "-Wno-sometimes-uninitialized"]

ext = Extension(
  "public_trader",
  sources=["public_trader.py"],
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
