from cx_Freeze import setup, Executable

setup(
    name = "MethodOfHords",
    version = "0.1",
    description = "MethodOfHords",
    executables = [Executable("MethodofIterations.py")]
)