// ============================================================
// Computer Graphics Mini Project: Shape Morphing Animation
// Subject : Object Oriented Programming and Computer Graphics (R25-CO-PCC-203)
//
// Syllabus algorithms used (all implemented by hand on a software
// frame buffer; OpenGL is only used to put the finished pixels on screen):
//   Unit 4  Scan conversion : DDA line, Bresenham line,
//                             DDA circle, Bresenham circle, Midpoint circle
//   Unit 5  Polygons        : inside test (even-odd), scan-line fill,
//                             flood fill, seed (boundary) fill
//           Clipping        : Cohen-Sutherland (lines),
//                             Sutherland-Hodgman (polygons)
//           Animation       : key-frame animation, morphing (interpolation),
//                             motion specification (path), easing
//
// Compile (Linux)  : g++ cg_shape_morphing.cpp -o morph -lGL -lGLU -lglut
// Compile (Windows, MinGW + freeglut):
//   g++ cg_shape_morphing.cpp -o morph -lfreeglut -lopengl32 -lglu32
//
// Controls:
//   SPACE pause     N next key-frame   + / -  speed
//   L  line algorithm   : DDA <-> Bresenham
//   F  fill algorithm   : Scan-line -> Flood fill -> Seed (boundary) fill
//   C  clipping on/off  : Cohen-Sutherland + Sutherland-Hodgman window
//   M  motion path on/off
//   G  guide circle     : off -> Midpoint -> Bresenham -> DDA
//   R  reset            ESC quit
// ============================================================
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;

const int   W = 700, H = 700;     // window = frame buffer size (pixels)
const int   N = 120;              // vertices in every key-frame shape
const float PI = 3.14159265f;
const float WORLD = 3.0f;         // world coordinates span -1.5 .. +1.5

struct Pt  { float x, y; };
struct Col { float r, g, b; };
struct RGB { unsigned char r, g, b; };
bool operator==(const RGB &a, const RGB &b) { return a.r == b.r && a.g == b.g && a.b == b.b; }
bool operator!=(const RGB &a, const RGB &b) { return !(a == b); }

// ---------------- Software frame buffer ----------------
static unsigned char fb[H][W][3];
const RGB BG = {18, 20, 31}, WHITE = {255, 255, 255}, YELLOW = {255, 215, 0}, GRAY = {105, 112, 140};

void putPixel(int x, int y, RGB c) {
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    fb[y][x][0] = c.r; fb[y][x][1] = c.g; fb[y][x][2] = c.b;
}
RGB getPixel(int x, int y) {
    if (x < 0 || x >= W || y < 0 || y >= H) return {0, 0, 0};
    return {fb[y][x][0], fb[y][x][1], fb[y][x][2]};
}
void clearBuffer() {
    for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) putPixel(x, y, BG);
}

// ============================================================
//  UNIT 4 : LINE DRAWING
// ============================================================
// DDA: step = max(|dx|,|dy|); increment x and y by dx/step, dy/step
void ddaLine(int x1, int y1, int x2, int y2, RGB c) {
    int dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    if (steps == 0) { putPixel(x1, y1, c); return; }
    float xi = dx / (float)steps, yi = dy / (float)steps, x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        putPixel((int)lroundf(x), (int)lroundf(y), c);
        x += xi; y += yi;
    }
}

// Bresenham: integer-only error term, works in all octants
void bresenhamLine(int x0, int y0, int x1, int y1, RGB c) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        putPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

bool useBresenham = true;
void line(int x0, int y0, int x1, int y1, RGB c) {
    if (useBresenham) bresenhamLine(x0, y0, x1, y1, c);
    else              ddaLine(x0, y0, x1, y1, c);
}

// ============================================================
//  UNIT 4 : CIRCLE DRAWING
// ============================================================
void plot8(int cx, int cy, int x, int y, RGB c) {      // 8-way symmetry
    putPixel(cx + x, cy + y, c); putPixel(cx - x, cy + y, c);
    putPixel(cx + x, cy - y, c); putPixel(cx - x, cy - y, c);
    putPixel(cx + y, cy + x, c); putPixel(cx - y, cy + x, c);
    putPixel(cx + y, cy - x, c); putPixel(cx - y, cy - x, c);
}
// Midpoint circle: decision parameter p = 1 - r
void midpointCircle(int cx, int cy, int r, RGB c) {
    int x = 0, y = r, p = 1 - r;
    plot8(cx, cy, x, y, c);
    while (x < y) {
        x++;
        if (p < 0) p += 2 * x + 1;
        else { y--; p += 2 * (x - y) + 1; }
        plot8(cx, cy, x, y, c);
    }
}
// Bresenham circle: decision parameter d = 3 - 2r
void bresenhamCircle(int cx, int cy, int r, RGB c) {
    int x = 0, y = r, d = 3 - 2 * r;
    plot8(cx, cy, x, y, c);
    while (y >= x) {
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
        else d = d + 4 * x + 6;
        plot8(cx, cy, x, y, c);
    }
}
// DDA circle: x' = x + eps*y ; y' = y - eps*x'   with eps = 1/2^k, 2^(k-1) <= r < 2^k
void ddaCircle(int cx, int cy, int r, RGB c) {
    float eps = 1.0f;
    while (eps * r >= 0.5f) eps /= 2;           // eps ~ 1 / 2^k
    float x = r, y = 0;
    int steps = (int)(2 * PI / eps) + 2;
    for (int i = 0; i < steps; i++) {
        putPixel(cx + (int)lroundf(x), cy + (int)lroundf(y), c);
        x = x + eps * y;
        y = y - eps * x;
    }
}
int guideMode = 0;   // 0 off, 1 midpoint, 2 bresenham, 3 DDA
const char *guideName[] = {"Off", "Midpoint", "Bresenham", "DDA"};

// ============================================================
//  UNIT 5 : CLIPPING   (clip window in pixel coordinates)
// ============================================================
const float XMIN = 200, XMAX = 500, YMIN = 200, YMAX = 500;
bool clipOn = false;

// ---- Cohen-Sutherland line clipping ----
const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;
int outcode(float x, float y) {
    int c = 0;
    if (x < XMIN) c |= LEFT; else if (x > XMAX) c |= RIGHT;
    if (y < YMIN) c |= BOTTOM; else if (y > YMAX) c |= TOP;
    return c;
}
bool cohenSutherland(float &x0, float &y0, float &x1, float &y1) {
    int c0 = outcode(x0, y0), c1 = outcode(x1, y1);
    while (true) {
        if (!(c0 | c1)) return true;      // trivially accepted
        if (c0 & c1)    return false;     // trivially rejected
        int co = c0 ? c0 : c1;
        float x, y;
        if (co & TOP)         { x = x0 + (x1 - x0) * (YMAX - y0) / (y1 - y0); y = YMAX; }
        else if (co & BOTTOM) { x = x0 + (x1 - x0) * (YMIN - y0) / (y1 - y0); y = YMIN; }
        else if (co & RIGHT)  { y = y0 + (y1 - y0) * (XMAX - x0) / (x1 - x0); x = XMAX; }
        else                  { y = y0 + (y1 - y0) * (XMIN - x0) / (x1 - x0); x = XMIN; }
        if (co == c0) { x0 = x; y0 = y; c0 = outcode(x0, y0); }
        else          { x1 = x; y1 = y; c1 = outcode(x1, y1); }
    }
}

// ---- Sutherland-Hodgman polygon clipping (one edge of window at a time) ----
bool insideEdge(const Pt &p, int e) {
    switch (e) { case 0: return p.x >= XMIN; case 1: return p.x <= XMAX;
                 case 2: return p.y >= YMIN; default: return p.y <= YMAX; }
}
Pt intersectEdge(const Pt &a, const Pt &b, int e) {
    float t;
    switch (e) {
        case 0:  t = (XMIN - a.x) / (b.x - a.x); return {XMIN, a.y + t * (b.y - a.y)};
        case 1:  t = (XMAX - a.x) / (b.x - a.x); return {XMAX, a.y + t * (b.y - a.y)};
        case 2:  t = (YMIN - a.y) / (b.y - a.y); return {a.x + t * (b.x - a.x), YMIN};
        default: t = (YMAX - a.y) / (b.y - a.y); return {a.x + t * (b.x - a.x), YMAX};
    }
}
vector<Pt> sutherlandHodgman(vector<Pt> in) {
    for (int e = 0; e < 4 && !in.empty(); e++) {
        vector<Pt> out;
        int m = in.size();
        for (int i = 0; i < m; i++) {
            Pt cur = in[i], prev = in[(i + m - 1) % m];
            bool ci = insideEdge(cur, e), pi = insideEdge(prev, e);
            if (ci) { if (!pi) out.push_back(intersectEdge(prev, cur, e)); out.push_back(cur); }
            else if (pi) out.push_back(intersectEdge(prev, cur, e));
        }
        in = out;
    }
    return in;
}

// ============================================================
//  UNIT 5 : POLYGON FILLING & INSIDE TEST
// ============================================================
// Even-odd inside test (ray casting)
bool insideTest(const vector<Pt> &p, float x, float y) {
    bool in = false;
    int m = p.size();
    for (int i = 0, j = m - 1; i < m; j = i++)
        if (((p[i].y > y) != (p[j].y > y)) &&
            (x < (p[j].x - p[i].x) * (y - p[i].y) / (p[j].y - p[i].y) + p[i].x))
            in = !in;
    return in;
}

// Scan-line fill: for every row find edge intersections, sort, fill in pairs
void scanLineFill(const vector<Pt> &p, RGB c) {
    int m = p.size();
    if (m < 3) return;
    float ymin = 1e9f, ymax = -1e9f;
    for (auto &q : p) { ymin = min(ymin, q.y); ymax = max(ymax, q.y); }
    for (int y = max(0, (int)floorf(ymin)); y <= min(H - 1, (int)ceilf(ymax)); y++) {
        float sy = y + 0.5f;
        vector<float> xs;
        for (int i = 0; i < m; i++) {
            const Pt &a = p[i], &b = p[(i + 1) % m];
            if ((a.y <= sy && b.y > sy) || (b.y <= sy && a.y > sy))
                xs.push_back(a.x + (sy - a.y) * (b.x - a.x) / (b.y - a.y));
        }
        sort(xs.begin(), xs.end());
        for (size_t k = 0; k + 1 < xs.size(); k += 2)
            for (int x = (int)ceilf(xs[k] - 0.5f); x <= (int)floorf(xs[k + 1] - 0.5f); x++)
                putPixel(x, y, c);
    }
}

// Flood fill (4-connected): replaces every pixel having the OLD colour
void floodFill(int sx, int sy, RGB fill, RGB old) {
    if (old == fill || getPixel(sx, sy) != old) return;
    vector<int> st; st.reserve(1 << 16);
    putPixel(sx, sy, fill); st.push_back(sy * W + sx);
    const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    while (!st.empty()) {
        int v = st.back(); st.pop_back();
        int x = v % W, y = v / W;
        for (int k = 0; k < 4; k++) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (getPixel(nx, ny) == old) { putPixel(nx, ny, fill); st.push_back(ny * W + nx); }
        }
    }
}

// Seed / boundary fill (4-connected): spreads from the seed until the BOUNDARY colour
void boundaryFill(int sx, int sy, RGB fill, RGB boundary) {
    RGB s = getPixel(sx, sy);
    if (s == boundary || s == fill) return;
    vector<int> st; st.reserve(1 << 16);
    putPixel(sx, sy, fill); st.push_back(sy * W + sx);
    const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    while (!st.empty()) {
        int v = st.back(); st.pop_back();
        int x = v % W, y = v / W;
        for (int k = 0; k < 4; k++) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            RGB q = getPixel(nx, ny);
            if (q != boundary && q != fill) { putPixel(nx, ny, fill); st.push_back(ny * W + nx); }
        }
    }
}
int fillMode = 0;    // 0 scan-line, 1 flood fill, 2 seed (boundary) fill
const char *fillName[] = {"Scan-line", "Flood fill", "Seed (boundary) fill"};
bool seedInside = true;

// ============================================================
//  KEY-FRAME SHAPES (all resampled to N vertices)
// ============================================================
struct KeyFrame { vector<Pt> pts; Col color; const char *name; };

// Equal vertex count is required for vertex-to-vertex interpolation.
vector<Pt> resample(const vector<Pt> &poly, int n) {
    int m = poly.size();
    vector<float> cum(m + 1, 0);
    for (int i = 0; i < m; i++) {
        const Pt &a = poly[i], &b = poly[(i + 1) % m];
        cum[i + 1] = cum[i] + hypotf(b.x - a.x, b.y - a.y);
    }
    vector<Pt> out;
    int seg = 0;
    for (int i = 0; i < n; i++) {
        float d = cum[m] * i / n;
        while (seg < m - 1 && cum[seg + 1] < d) seg++;
        float t = (d - cum[seg]) / (cum[seg + 1] - cum[seg]);
        const Pt &a = poly[seg], &b = poly[(seg + 1) % m];
        out.push_back({a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t});
    }
    return out;
}
// every shape starts at the top and runs clockwise so the morph doesn't twist
vector<Pt> makeSquare()   { return resample({{0, .8f}, {.8f, .8f}, {.8f, -.8f}, {-.8f, -.8f}, {-.8f, .8f}}, N); }
vector<Pt> makeTriangle() { return resample({{0, .9f}, {.85f, -.65f}, {-.85f, -.65f}}, N); }
vector<Pt> makeStar() {
    vector<Pt> v;
    for (int i = 0; i < 10; i++) {
        float r = (i % 2 == 0) ? 0.95f : 0.4f, a = PI / 2 - i * PI / 5;
        v.push_back({r * cosf(a), r * sinf(a)});
    }
    return resample(v, N);
}
vector<Pt> makeCircle() {
    vector<Pt> v;
    for (int i = 0; i < N; i++) {
        float a = PI / 2 - 2 * PI * i / N;
        v.push_back({0.85f * cosf(a), 0.85f * sinf(a)});
    }
    return v;
}
vector<Pt> makeHeart() {
    vector<Pt> v;
    for (int i = 0; i < N; i++) {
        float t = 2 * PI * i / N;
        float x = 16 * powf(sinf(t), 3);
        float y = 13 * cosf(t) - 5 * cosf(2 * t) - 2 * cosf(3 * t) - cosf(4 * t);
        v.push_back({x / 18.0f, (y - 2) / 18.0f});
    }
    return v;
}

// ============================================================
//  ANIMATION STATE
// ============================================================
vector<KeyFrame> keys;
int   cur = 0;
float t = 0, holdTimer = 0, speed = 0.6f, clk = 0;
bool  paused = false, motionOn = false;
const float HOLD = 0.8f;

float smoothstep(float u) { return u * u * (3 - 2 * u); }      // easing

// Motion specification: position of the shape as a function of time
Pt motionOffset() {
    if (!motionOn) return {0, 0};
    return {0.30f * cosf(clk * 1.2f), 0.25f * sinf(clk * 2.4f)};   // figure-eight path
}

// ============================================================
//  RENDER ONE FRAME INTO THE FRAME BUFFER
// ============================================================
void render() {
    clearBuffer();
    int next = (cur + 1) % keys.size();
    float e = smoothstep(t);
    const KeyFrame &A = keys[cur], &B = keys[next];
    Pt off = motionOffset();

    // 1. Morphing: P(t) = (1-e)*P_start + e*P_end   (vertex + colour)
    vector<Pt> p(N);
    for (int i = 0; i < N; i++) {
        float x = (1 - e) * A.pts[i].x + e * B.pts[i].x + off.x;
        float y = (1 - e) * A.pts[i].y + e * B.pts[i].y + off.y;
        p[i] = {(x + WORLD / 2) / WORLD * W, (y + WORLD / 2) / WORLD * H};   // world -> pixel
    }
    RGB fillC = {(unsigned char)(255 * ((1 - e) * A.color.r + e * B.color.r)),
                 (unsigned char)(255 * ((1 - e) * A.color.g + e * B.color.g)),
                 (unsigned char)(255 * ((1 - e) * A.color.b + e * B.color.b))};

    // 2. Optional guide circle (Midpoint / Bresenham / DDA circle algorithm)
    auto drawGuide = [&]() {
        int r = (int)(0.85f / WORLD * W), cx = W / 2, cy = H / 2;
        if (guideMode == 1) midpointCircle(cx, cy, r, GRAY);
        if (guideMode == 2) bresenhamCircle(cx, cy, r, GRAY);
        if (guideMode == 3) ddaCircle(cx, cy, r, GRAY);
    };

    // 3. Outline: every edge drawn with DDA / Bresenham (Cohen-Sutherland if clipping)
    auto drawOutline = [&]() {
        for (int i = 0; i < N; i++) {
            float x0 = p[i].x, y0 = p[i].y, x1 = p[(i + 1) % N].x, y1 = p[(i + 1) % N].y;
            if (clipOn && !cohenSutherland(x0, y0, x1, y1)) continue;
            line((int)lroundf(x0), (int)lroundf(y0), (int)lroundf(x1), (int)lroundf(y1), WHITE);
        }
    };

    seedInside = true;
    if (clipOn) {
        // Sutherland-Hodgman clips the polygon, scan-line fills what is left
        scanLineFill(sutherlandHodgman(p), fillC);
        drawOutline();
        line((int)XMIN, (int)YMIN, (int)XMAX, (int)YMIN, YELLOW);
        line((int)XMAX, (int)YMIN, (int)XMAX, (int)YMAX, YELLOW);
        line((int)XMAX, (int)YMAX, (int)XMIN, (int)YMAX, YELLOW);
        line((int)XMIN, (int)YMAX, (int)XMIN, (int)YMIN, YELLOW);
    } else if (fillMode == 0) {
        scanLineFill(p, fillC);
        drawOutline();
    } else {
        drawOutline();                           // boundary must exist first
        // seed point = centroid, verified with the inside test
        float cx = 0, cy = 0;
        for (auto &q : p) { cx += q.x; cy += q.y; }
        cx /= N; cy /= N;
        seedInside = insideTest(p, cx, cy);
        if (!seedInside) {                       // fall back: middle of first span on row cy
            vector<float> xs;
            for (int i = 0; i < N; i++) {
                const Pt &a = p[i], &b = p[(i + 1) % N];
                if ((a.y <= cy && b.y > cy) || (b.y <= cy && a.y > cy))
                    xs.push_back(a.x + (cy - a.y) * (b.x - a.x) / (b.y - a.y));
            }
            sort(xs.begin(), xs.end());
            if (xs.size() >= 2) { cx = (xs[0] + xs[1]) / 2; seedInside = true; }
        }
        if (seedInside) {
            if (fillMode == 1) floodFill((int)cx, (int)cy, fillC, BG);
            else               boundaryFill((int)cx, (int)cy, fillC, WHITE);
        }
    }
    drawGuide();
}

// ============================================================
//  GLUT CALLBACKS
// ============================================================
void text(int x, int y, const char *s) {
    glRasterPos2i(x, y);
    for (; *s; s++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);
}

void display() {
    render();
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, fb);

    glColor3f(0.92f, 0.92f, 0.92f);
    char buf[200];
    int next = (cur + 1) % keys.size();
    snprintf(buf, sizeof buf, "%s -> %s    t = %.2f    %s", keys[cur].name, keys[next].name, t, paused ? "[PAUSED]" : "");
    text(12, H - 20, buf);
    snprintf(buf, sizeof buf, "Line: %s | Fill: %s | Clip: %s | Motion: %s | Guide circle: %s",
             useBresenham ? "Bresenham" : "DDA", clipOn ? "Scan-line (clipped)" : fillName[fillMode],
             clipOn ? "ON (Cohen-Sutherland + Sutherland-Hodgman)" : "OFF", motionOn ? "ON" : "OFF", guideName[guideMode]);
    text(12, H - 38, buf);
    if (!clipOn && fillMode != 0) {
        snprintf(buf, sizeof buf, "Seed point inside test: %s", seedInside ? "INSIDE" : "OUTSIDE");
        text(12, H - 56, buf);
    }
    text(12, 12, "SPACE pause  N next  +/- speed  L line  F fill  C clip  M motion  G guide circle  R reset");
    glutSwapBuffers();
}

void update(int) {
    const float dt = 0.016f;
    if (!paused) {
        clk += dt;
        if (holdTimer > 0) holdTimer -= dt;
        else {
            t += dt * speed;
            if (t >= 1) { t = 0; cur = (cur + 1) % keys.size(); holdTimer = HOLD; }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char k, int, int) {
    switch (k) {
        case 27:  exit(0);
        case ' ': paused = !paused; break;
        case 'n': case 'N': cur = (cur + 1) % keys.size(); t = 0; holdTimer = HOLD; break;
        case '+': case '=': speed *= 1.25f; break;
        case '-': case '_': speed /= 1.25f; break;
        case 'l': case 'L': useBresenham = !useBresenham; break;
        case 'f': case 'F': fillMode = (fillMode + 1) % 3; break;
        case 'c': case 'C': clipOn = !clipOn; break;
        case 'm': case 'M': motionOn = !motionOn; break;
        case 'g': case 'G': guideMode = (guideMode + 1) % 4; break;
        case 'r': case 'R': cur = 0; t = 0; holdTimer = HOLD; speed = 0.6f; clk = 0; break;
    }
}

void reshape(int w, int h) { if (w != W || h != H) glutReshapeWindow(W, H); }

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, W, 0, H);                    // 1 unit = 1 pixel
    glMatrixMode(GL_MODELVIEW);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    keys.push_back({makeSquare(),   {0.20f, 0.55f, 0.95f}, "Square"});
    keys.push_back({makeTriangle(), {0.95f, 0.45f, 0.20f}, "Triangle"});
    keys.push_back({makeStar(),     {0.98f, 0.85f, 0.15f}, "Star"});
    keys.push_back({makeCircle(),   {0.25f, 0.85f, 0.45f}, "Circle"});
    keys.push_back({makeHeart(),    {0.92f, 0.25f, 0.45f}, "Heart"});
    holdTimer = HOLD;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("Shape Morphing Animation - CG Mini Project");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
