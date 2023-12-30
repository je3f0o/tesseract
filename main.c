#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#define YUP (Vector3){ 0.0f, 1.0f, 0.0f }
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

float TAU = PI * 2;

const int screenWidth  = 800;
const int screenHeight = 600;

typedef int ivec2[2];

typedef float Matrix43[12];

Camera3D camera = {
  .position   = (Vector3){ 0.0f, 5.0f, 10.0f },
  .target     = (Vector3){ 0.0f, 0.0f, 0.0f },
  .up         = (Vector3){ 0.0f, 1.0f, 0.0f },
  .fovy       = 45.0f,
  .projection = CAMERA_PERSPECTIVE,
};

const Vector4 points[] = {
  // Inner cube
  {-1, -1, -1, +1},
  {+1, -1, -1, +1},
  {+1, +1, -1, +1},
  {-1, +1, -1, +1},
  {-1, -1, +1, +1},
  {+1, -1, +1, +1},
  {+1, +1, +1, +1},
  {-1, +1, +1, +1},

  // Outer cube
  {-1, -1, -1, -1},
  {+1, -1, -1, -1},
  {+1, +1, -1, -1},
  {-1, +1, -1, -1},
  {-1, -1, +1, -1},
  {+1, -1, +1, -1},
  {+1, +1, +1, -1},
  {-1, +1, +1, -1},
};

Vector3 points_3[16] = {0};

ivec2 indices[] = {
  // Bottom lines
  {0, 1},
  {1, 2},
  {2, 3},
  {3, 0},

  // Top lines
  {4, 5},
  {5, 6},
  {6, 7},
  {7, 4},

  // Vertical lines
  {0, 0 + 4},
  {1, 1 + 4},
  {2, 2 + 4},
  {3, 3 + 4},
};

Vector4 mat4_mul_vec4(Matrix m, Vector4 v) {
  Vector4 result;
  result.x = m.m0*v.x + m.m4*v.y + m.m8 *v.z + m.m12*v.w;
  result.y = m.m1*v.x + m.m5*v.y + m.m9 *v.z + m.m13*v.w;
  result.z = m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14*v.w;
  result.w = m.m3*v.x + m.m7*v.y + m.m11*v.z + m.m15*v.w;
  return result;
}

Vector3 mat43_mul_vec4(Matrix43 m, Vector4 v) {
  Vector3 result;
  result.x = m[0]*v.x + m[1]*v.y + m[2] *v.z + m[3]*v.w;
  result.y = m[4]*v.x + m[5]*v.y + m[6] *v.z + m[7]*v.w;
  result.z = m[8]*v.x + m[9]*v.y + m[10] *v.z + m[11]*v.w;
  return result;
}

Vector4 Vector4Scale(Vector4 v, float s) {
  return (Vector4){ v.x*s, v.y*s, v.z*s, v.w };
}

void update(void) {
  static float angle = 0;
  angle += 0.01;
  float c = cosf(angle);
  float s = sinf(angle);
  Matrix rotXY = {
    c, -s, 0, 0,
    s,  c, 0, 0,
    0,  0, 1, 0,
    0,  0, 0, 1
  };
  Matrix rotZW = {
    1, 0,  0,  0,
    0, 1,  0,  0,
    0, 0,  c, -s,
    0, 0,  s,  c
  };

  for (size_t i = 0; i < ARRAY_LEN(points); ++i) {
    Vector4 v4 = points[i];
    v4 = mat4_mul_vec4(rotXY, v4);
    v4 = mat4_mul_vec4(rotZW, v4);

    float w = 1. / (2. - v4.w);
    Matrix43 final_transform = {
      w, 0, 0, 0,
      0, w, 0, 0,
      0, 0, w, 0,
    };

    Vector3 v3 = mat43_mul_vec4(final_transform, v4);
    points_3[i] = Vector3Scale(v3, 2);
    points_3[i] = Vector3Add(points_3[i], (Vector3){0, 5, 0});
  }
}

void draw_tesseract(void) {
  for (size_t i = 0; i < ARRAY_LEN(points_3); ++i) {
    Color color = i > 7 ? BLUE : RED;
    DrawSphere(points_3[i], 0.1, color);
  }

  // Inner cube lines
  for (size_t i = 0; i < ARRAY_LEN(indices); ++i) {
    int i0 = indices[i][0];
    int i1 = indices[i][1];
    DrawLine3D(points_3[i0], points_3[i1], RED);
  }

  // Outer cube lines
  for (size_t i = 0; i < ARRAY_LEN(indices); ++i) {
    int i0 = indices[i][0] + 8;
    int i1 = indices[i][1] + 8;
    DrawLine3D(points_3[i0], points_3[i1], BLUE);
  }

  // Diagnal cube lines
  for (size_t i = 0; i < 8; ++i) {
    int i0 = indices[i][0];
    int i1 = i0 + 8;
    DrawLine3D(points_3[i0], points_3[i1], GREEN);
  }
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera free");

  // Define the camera to look into our 3d world
  SetTargetFPS(60);

  bool pause = false;

  while (!WindowShouldClose()) {
    if (!pause) update();
      
    UpdateCamera(&camera, CAMERA_FREE);

    if (IsKeyPressed('Z')) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    if (IsKeyDown('R')) camera.position.y += 0.1;
    if (IsKeyDown('F')) camera.position.y -= 0.1;
    if (IsKeyPressed(KEY_SPACE)) pause = !pause;

    // Render
    BeginDrawing();
      ClearBackground(BLACK);
      BeginMode3D(camera);
        draw_tesseract();
        DrawGrid(20, 1.0f);
      EndMode3D();
    EndDrawing();
  }
  CloseWindow();

  return 0;
}