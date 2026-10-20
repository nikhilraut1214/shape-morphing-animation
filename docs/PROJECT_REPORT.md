# Computer Graphics Mini Project Report
## Shape Morphing Animation

### 1. Abstract

Shape Morphing Animation is a C++ computer-graphics project that demonstrates the transformation of one geometric shape into another through key-frame animation and vertex interpolation. It transitions between a square, triangle, star, circle, and heart. Alongside animation, the project demonstrates line and circle rasterization, polygon filling, point-in-polygon testing, and clipping algorithms. A software RGB frame buffer stores the generated pixels, while OpenGL/GLUT displays the result in an interactive desktop window.

### 2. Objectives

- Demonstrate shape transformation using key frames and interpolation.
- Implement and compare line-drawing and circle-drawing algorithms.
- Demonstrate scan-line polygon filling and seed-based region filling.
- Apply line and polygon clipping against a rectangular window.
- Provide keyboard controls for interactive experimentation.
- Connect theoretical Computer Graphics concepts to a working visual program.

### 3. Software and technologies

- **Programming language:** C++
- **Language standard:** C++11 or later
- **Graphics/display:** OpenGL, GLU, GLUT/freeglut
- **Development environment used in the supplied run instructions:** MSYS2 UCRT64 with MinGW-w64 `g++`
- **Rendering approach:** Software RGB frame buffer displayed through OpenGL

### 4. System design

The application can be understood as these stages:

1. Initialize the GLUT window and graphics state.
2. Construct the key-frame polygons.
3. Resample each polygon to a common vertex count.
4. Update animation time and transition progress.
5. Interpolate vertices and colours between adjacent key frames.
6. Apply optional motion offset.
7. Rasterize the outline and fill the polygon using the selected modes.
8. Optionally clip geometry to the rectangular clipping window.
9. Display the frame buffer and current state information.
10. Repeat through the timer-driven update loop.

### 5. Algorithms implemented

- DDA line drawing
- Bresenham line drawing
- DDA circle drawing
- Bresenham circle drawing
- Midpoint circle drawing
- Even–odd point-in-polygon test
- Scan-line polygon filling
- Flood filling
- Seed/boundary filling
- Cohen–Sutherland line clipping
- Sutherland–Hodgman polygon clipping
- Key-frame vertex interpolation
- Smoothstep easing
- Parametric figure-eight-like motion

See [`ALGORITHMS.md`](ALGORITHMS.md) for the implementation summary.

### 6. User interaction

| Key | Function |
|---|---|
| `Space` | Pause/resume |
| `N` | Advance to next key frame |
| `+` / `-` | Adjust speed |
| `L` | Switch line algorithm |
| `F` | Cycle fill mode |
| `C` | Toggle clipping |
| `M` | Toggle motion |
| `G` | Cycle guide-circle mode |
| `R` | Reset animation |
| `Esc` | Exit |

### 7. Expected output

When launched, the program opens a 700 × 700 graphics window. A coloured shape morphs through the five key frames in a continuous cycle. On-screen status text identifies the current transition and active drawing/filling/clipping/motion settings. Keyboard controls let the user explore the different algorithm modes.

### 8. How to compile and execute

For Windows using the MSYS2 UCRT64 terminal, install freeglut if required:

```bash
pacman -S mingw-w64-ucrt-x86_64-freeglut
```

From the repository root, compile and run:

```bash
g++ -std=c++11 src/cg_shape_morphing.cpp -o morph.exe -lfreeglut -lopengl32 -lglu32
./morph.exe
```

For Linux systems with the relevant development libraries installed:

```bash
g++ -std=c++11 src/cg_shape_morphing.cpp -o morph -lGL -lGLU -lglut
./morph
```

### 9. Conclusion

The project combines geometric modelling, rasterization, filling, clipping, and animation in a single interactive demonstration. It illustrates how foundational graphics algorithms can be implemented directly and then presented through a graphics window, making the concepts easier to observe and experiment with.

### 10. Future scope

Potential extensions include loading custom polygon shapes, adding more key frames, offering adjustable animation parameters, exporting frames, adding a graphical control panel, and improving the morphing correspondence between dissimilar shapes.

### 11. Project author

- **Name:** Replace with your name
- **Course / subject:** Replace with your course details
- **Institution:** Replace with your institution
- **GitHub:** Add your profile or repository link
