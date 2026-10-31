# Unified Engineering Workbench — starter

This is a minimal native Windows desktop starter using the Win32 API and C++17.
It deliberately avoids Qt because Qt is not installed and the deadline is close.

## Build with MinGW
Open Antigravity terminal (PowerShell) in this folder:

```powershell
g++ --version
g++ -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++ -Iinclude src/main.cpp src/thermal/thermal_solver.cpp -o Launch.exe -mwindows -lcomctl32
```

Run:
```powershell
.\Launch.exe
```

If `g++` is not recognized, configure the MinGW `bin` directory in PATH first.
The static flags reduce runtime DLL requirements but do not guarantee a clean-machine
launch; verify on a second Windows environment before submission.

## Current scope
- Native Win32 window and buttons.
- Working 1D steady-state thermal FEM solver.
- Exact analytical comparison at element midpoints.
- Thermal CSV export to `results/thermal_N10.csv` etc.

## Next modules
Geometry STL/CSV parsing, parallel worker pool, hardware mock adapters,
backpropagation, automated tests, report, and packaging.
