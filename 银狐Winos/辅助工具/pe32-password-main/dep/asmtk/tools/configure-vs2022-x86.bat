@echo off
cmake .. -B "..\build_vs2022_x86" -G"Visual Studio 17" -A Win32 -DASMTK_TEST=1
