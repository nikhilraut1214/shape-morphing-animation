# Shape Morphing Animation — Computer Graphics Mini Project

A C++ computer-graphics mini project that demonstrates **shape morphing, rasterization algorithms, polygon filling, clipping, and animation** using a software frame buffer displayed through OpenGL/GLUT.

The animation transitions between five shapes—**Square, Triangle, Star, Circle, and Heart**—while allowing the viewer to switch drawing, filling, clipping, and motion options at runtime.

> **Project type:** Academic Computer Graphics Mini Project  
> **Language:** C++  
> **Graphics/display libraries:** OpenGL, GLU, GLUT/freeglut  
> **Window/frame-buffer size:** 700 × 700 pixels

## Contents

- [Overview](#overview)
- [Features](#features)
- [Algorithms demonstrated](#algorithms-demonstrated)
- [How the animation works](#how-the-animation-works)
- [Requirements](#requirements)
- [Build and run](#build-and-run)
- [Keyboard controls](#keyboard-controls)
- [Project structure](#project-structure)
- [Demo video](#demo-video)
- [Learning outcomes](#learning-outcomes)
- [Limitations and possible improvements](#limitations-and-possible-improvements)
- [Troubleshooting](#troubleshooting)
- [Author](#author)
- [License](#license)

## Overview

Shape Morphing Animation illustrates how one polygon can gradually transform into another by interpolating corresponding vertices over time. The project includes a small software rasterization pipeline: algorithms calculate pixels in a software frame buffer, and OpenGL is used to display the completed pixel buffer in a window.

It also provides interactive demonstrations of line and circle drawing, polygon filling, and line/polygon clipping. This makes the project useful for exploring foundational Computer Graphics concepts in a visual, interactive way.

## Features

- Automatic morphing through five key-frame shapes:
  - Square
  - Triangle
  - Star
  - Circle
  - Heart
- Smoothstep easing for less abrupt transitions.
- Interpolation of both vertex positions and shape colours.
- Optional figure-eight motion path.
- Switchable DDA and Bresenham line drawing.
- Switchable scan-line, flood-fill, and seed/boundary-fill modes.
- Cohen–Sutherland line clipping.
- Sutherland–Hodgman polygon clipping.
- Optional guide circle using Midpoint, Bresenham, or DDA circle drawing.
- Keyboard controls to pause, change speed, step through key frames, toggle modes, and reset the animation.
- A software RGB frame buffer with a dark background and contrasting shape outlines.

## Algorithms demonstrated

| Concept | Algorithm | Purpose in this project |
|---|---|---|
| Line rasterization | Digital Differential Analyzer (DDA) | Draws line segments using incremental floating-point steps |
| Line rasterization | Bresenham line algorithm | Draws line segments using an integer error term |
| Circle rasterization | Midpoint circle algorithm | Draws a circle using a decision parameter and eight-way symmetry |
| Circle rasterization | Bresenham circle algorithm | Draws a circle using incremental decision updates |
| Circle rasterization | DDA circle algorithm | Generates circle pixels through incremental coordinate updates |
| Point-in-polygon | Even–odd / ray-casting test | Checks whether a point is inside a polygon |
| Polygon filling | Scan-line fill | Finds edge intersections on each row and fills between intersection pairs |
| Region filling | Flood fill | Replaces connected pixels of a target background colour |
| Region filling | Seed/boundary fill | Fills outward from a seed until the boundary colour is reached |
| Line clipping | Cohen–Sutherland | Accepts, rejects, or clips a line against a rectangular window |
| Polygon clipping | Sutherland–Hodgman | Clips a polygon against the four edges of a rectangular window |
| Animation | Key frames and vertex interpolation | Transitions between corresponding shape vertices |
| Animation | Smoothstep easing | Smooths the interpolation progress |
| Motion | Parametric path | Moves the shape along an optional figure-eight path |

## How the animation works

1. **Create key frames.** The program defines the five shapes as polygon vertex lists.
2. **Normalize vertex counts.** Each shape is resampled to 120 points along its perimeter. Matching vertex counts make vertex-to-vertex interpolation possible.
3. **Interpolate vertices.** During a transition, each source vertex moves toward the corresponding destination vertex according to the animation progress.
4. **Apply easing.** The smoothstep function `u * u * (3 - 2 * u)` is used to ease the transition.
5. **Interpolate colour.** The RGB colour values transition between the two key frames as well.
6. **Rasterize the shape.** The selected fill and line algorithms draw the shape into the software frame buffer.
7. **Display the frame.** `glDrawPixels` displays the finished RGB buffer through the OpenGL window.
8. **Repeat.** The timer updates animation progress and advances to the next key frame, creating a continuous cycle.

The source code uses a 700 × 700 RGB frame buffer. OpenGL/GLUT handles the window and display callbacks; the listed rasterization, filling, and clipping routines are implemented in the C++ source.

## Requirements

- C++ compiler with C++11 support (for example, `g++`)
- OpenGL development libraries
- GLU development library
- GLUT or freeglut development headers and libraries
- Windows with MSYS2 UCRT64, Linux, or another environment configured with compatible OpenGL/GLUT libraries

## Build and run

### Windows — MSYS2 UCRT64

The following commands match the MSYS2 UCRT64 environment. Open the **MSYS2 UCRT64** terminal.

1. Install freeglut if it is not already installed:

   ```bash
   pacman -S mingw-w64-ucrt-x86_64-freeglut
   ```

2. Navigate to the project directory. Replace the example path with the location where you cloned or extracted this repository:

   ```bash
   cd "/d/CG mini project"
   ```

   If you cloned the repository into another directory, use that directory instead.

3. Compile the program:

   ```bash
   g++ -std=c++11 src/cg_shape_morphing.cpp -o morph.exe -lfreeglut -lopengl32 -lglu32
   ```

4. Run it:

   ```bash
   ./morph.exe
   ```

If you keep the source file in the repository root rather than `src/`, use `cg_shape_morphing.cpp` instead of `src/cg_shape_morphing.cpp` in the compile command.

### Linux

Install the C++ compiler and the OpenGL, GLU, and GLUT development packages using your distribution's package manager. For Debian/Ubuntu-based distributions, the package names are commonly `build-essential`, `freeglut3-dev`, and `libglu1-mesa-dev`.

Compile and run from the repository root:

```bash
g++ -std=c++11 src/cg_shape_morphing.cpp -o morph -lGL -lGLU -lglut
./morph
```

Package names can vary by Linux distribution.

## Keyboard controls

| Key | Action |
|---|---|
| `Space` | Pause or resume animation |
| `N` | Jump to the next key frame |
| `+` or `=` | Increase animation speed |
| `-` or `_` | Decrease animation speed |
| `L` | Toggle line drawing between DDA and Bresenham |
| `F` | Cycle through Scan-line → Flood fill → Seed (boundary) fill |
| `C` | Toggle clipping on/off |
| `M` | Toggle figure-eight motion on/off |
| `G` | Cycle guide circle: Off → Midpoint → Bresenham → DDA |
| `R` | Reset animation position, progress, speed, and clock |
| `Esc` | Exit the application |

**Tip:** Try `L`, `F`, `C`, `M`, and `G` while the animation is running to explore the different algorithms.

## Project structure

```text
shape-morphing-animation/
├── README.md
├── .gitignore
├── src/
│   └── cg_shape_morphing.cpp
└── docs/
    ├── ALGORITHMS.md
    └── PROJECT_REPORT.md
```

Add your demo video or a link to it after uploading it (see [Demo video](#demo-video)).

## 🎬 Demo Video

Watch the demonstration of the Shape Morphing Animation project, showcasing transitions between Square, Triangle, Star, Circle, and Heart.

▶️ **[Watch the Shape Morphing Animation Demo](media/shape-morphing-demo.mp4)**

The demo highlights the animation, interactive controls, and computer graphics algorithms implemented in this project.

## Learning outcomes

This project provides practical exposure to:

- Pixel-level drawing using a software frame buffer.
- Comparing incremental rasterization algorithms.
- Polygon geometry and point-in-polygon testing.
- Scan-line and seed-based filling techniques.
- Clipping line segments and polygons to a rectangular viewport.
- Key-frame animation, vertex correspondence, interpolation, and easing.
- Event-driven keyboard input and timer-based animation using GLUT.

## Limitations and possible improvements

- Morphing uses vertex-to-vertex interpolation after resampling. This is a straightforward educational technique; it does not guarantee topology-aware morphing or eliminate all possible distortions between very different shapes.
- The project uses a fixed 700 × 700 rendering buffer.
- The fill and clipping demonstrations use a rectangular clipping window defined in the source.
- The program is a local desktop demonstration; it does not currently load user-provided shapes or export animation frames.
- Possible future improvements include configurable shapes, adjustable window size, frame export, more key frames, a GUI control panel, and performance profiling.

## Troubleshooting

### `GL/glut.h: No such file or directory`

Install the appropriate GLUT/freeglut development package and make sure the compiler environment matches the installed library. In MSYS2, use the **UCRT64** terminal and install `mingw-w64-ucrt-x86_64-freeglut`.

### `undefined reference` errors during compilation

Check that the OpenGL, GLU, and freeglut link libraries are installed and that the compile command includes the required linker flags. In MSYS2 UCRT64, use:

```bash
g++ -std=c++11 src/cg_shape_morphing.cpp -o morph.exe -lfreeglut -lopengl32 -lglu32
```

### `No such file or directory` for the source file

Run `ls` to inspect the current directory. Confirm that you are in the repository root and that the source path in the compile command matches the actual folder structure.

### The animation window closes or does not appear

Run the executable from the terminal so error messages remain visible. Confirm that your desktop session supports graphical OpenGL applications and that the executable was built successfully.

## Author

**Your Name**  
Add your course, institution, and GitHub profile here if you want them shown publicly.

## License

No license has been selected for this repository. If you want others to reuse, modify, or distribute the project, choose a license and add its full text as a `LICENSE` file. Do not label the project as MIT-licensed unless you have actually added that license.
