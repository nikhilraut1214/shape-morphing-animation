# Algorithms and Implementation Notes

This document summarizes the computer-graphics algorithms implemented in `src/cg_shape_morphing.cpp`.

## 1. Software frame buffer

The program maintains an RGB array of 700 × 700 pixels. Drawing routines write individual pixels into this array, and the completed buffer is displayed with OpenGL's `glDrawPixels`. This separates the pixel-generation logic from the window/display mechanism.

## 2. Line drawing

### Digital Differential Analyzer (DDA)

The algorithm computes `dx` and `dy`, chooses the number of steps as `max(|dx|, |dy|)`, and advances the coordinates by `dx / steps` and `dy / steps`. Coordinates are rounded to pixel positions.

### Bresenham line algorithm

The implementation uses an integer error term to decide when to advance along each axis. It supports all octants and avoids the repeated floating-point increments used by DDA.

The `L` key switches between these two implementations.

## 3. Circle drawing

All circle routines use eight-way symmetry to plot equivalent points around the circle.

- **Midpoint circle:** starts with decision parameter `p = 1 - r` and updates it as the rasterized arc advances.
- **Bresenham circle:** starts with decision parameter `d = 3 - 2r` and updates the decision value incrementally.
- **DDA circle:** uses incremental coordinate updates with a step size derived from the radius.

Press `G` to cycle through Off, Midpoint, Bresenham, and DDA guide-circle modes.

## 4. Point-in-polygon test

The even–odd (ray-casting) test toggles an inside/outside state when a horizontal ray from the test point crosses polygon edges. It is used to validate a candidate seed point before filling.

## 5. Polygon filling

### Scan-line fill

For each pixel row intersecting the polygon, the algorithm calculates intersections with polygon edges, sorts the intersection x-coordinates, and fills between successive pairs. The main rendering path uses this method by default.

### Flood fill

The flood-fill routine uses a stack and four-connected neighbors. It replaces connected pixels that match a specified old colour with the new fill colour.

### Seed/boundary fill

The boundary-fill routine also uses a stack and four-connected neighbors. It expands from a seed pixel until it reaches the boundary colour or pixels already filled.

The `F` key cycles through scan-line, flood-fill, and seed/boundary-fill modes. When clipping is enabled, the program uses polygon clipping followed by scan-line filling.

## 6. Clipping

### Cohen–Sutherland line clipping

Each line endpoint is assigned an outcode describing whether it lies left, right, below, or above the rectangular clipping window. The algorithm quickly accepts fully inside segments, rejects segments that share an outside region, and iteratively intersects remaining segments with window boundaries.

### Sutherland–Hodgman polygon clipping

The polygon is clipped against each of the four window edges in sequence. For each edge, the algorithm considers whether the previous and current vertices are inside and emits the appropriate vertices and intersections.

Press `C` to toggle clipping. The clipping window is defined by constants in the source file.

## 7. Morphing and animation

The project defines five key-frame polygons: square, triangle, star, circle, and heart. Every shape is resampled to 120 vertices so the program can interpolate matching indices.

For source point `A` and destination point `B`, the interpolated point is:

`P(u) = (1 - e) A + e B`

where `e` is the eased animation progress. The code uses smoothstep easing:

`e = u²(3 - 2u)`

The RGB colour is interpolated using the same progress value. Once a transition completes, the animation advances to the next key frame and continues cyclically.

## 8. Optional motion path

When motion is enabled, a time-dependent offset is added to every interpolated vertex. The x and y offsets use sine and cosine functions with different frequencies, producing a figure-eight-like path.

## 9. Input and display loop

GLUT keyboard callbacks handle interactive controls. A timer callback updates the animation at approximately 16 ms intervals, requests a redraw, and schedules the next update. The display callback renders the frame buffer and writes status text describing the current algorithms and animation state.
