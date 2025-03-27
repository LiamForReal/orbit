from setuptools import setup, Extension
from Cython.Build import cythonize
from setuptools.command.build_ext import build_ext as _build_ext

# Custom build_ext that overrides the filename for the extension module
class build_ext(_build_ext):
    def get_ext_filename(self, ext_name):
        # Return just "gui.pyd" regardless of Python version or platform tags.
        return ext_name + ".pyd"

# Define your extension module (using your plain Python source file)
ext = Extension("gui", sources=["gui.py"])

setup(
    name="gui",
    ext_modules=cythonize(ext, compiler_directives={'language_level': "3"}),
    cmdclass={'build_ext': build_ext},
)
