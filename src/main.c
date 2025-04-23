#include <raylib.h>
#include <stdio.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define PIXEL_X 1280 // Screen width
#define PIXEL_Y 800  // Screen height
#define BALL_RADIUS 40
#define BALL_LEFT_LIMIT (BALL_RADIUS)
#define BALL_RIGHT_LIMIT (PIXEL_X - BALL_RADIUS)
#define JUMP_STRENGTH -25 // Lower number means a higher jump

typedef struct {
  float x, y, size, jump_strength, velocity;
} Ball;

typedef struct {
  float x, y, width, height;
} Platform;

Platform *ball_on_platform(Ball *ball, Platform *platform, int platform_count) {

  Vector2 center = {.x = (*ball).x, .y = (*ball).y};
  Rectangle bounding_box = {.x = (*ball).x - (*ball).size + 10,
                            .y = (*ball).y - (*ball).size,
                            .width = (*ball).size * 2 - 10,
                            .height = (*ball).size * 2 + 1};
  for (int i = 0; i < platform_count; i++) {
    Rectangle rectangle = *(Rectangle *)&platform[i];
    if (CheckCollisionRecs(bounding_box, rectangle)) {
      return &platform[i];
    }
  }
  return NULL;
}

void update_vertical_location(Ball *ball, Platform *platforms,
                              int platform_count) {
  Platform *on_platform = ball_on_platform(ball, platforms, platform_count);
  if (on_platform && (*ball).velocity == 0) {
    // If ball is on the platform with 0 velocity, do nothing
    return;
  } else {
    // change ball's y location based on current velocity
    ((*ball).velocity)++;
    (*ball).y = (*ball).y + (*ball).velocity;
    on_platform = ball_on_platform(ball, platforms, platform_count);
    // If ball was coming down and it hits a platform, stop it
    if (on_platform && (*ball).velocity > 0) {
      (*ball).velocity = 0;
      (*ball).y = (*on_platform).y - (*ball).size;
    }
  }
}

int main() {
  setvbuf(stdout, NULL, _IONBF, 0);

  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  InitWindow(PIXEL_X, PIXEL_Y, "cgame");

  Ball ball = {.x = PIXEL_X / 2,
               .y = PIXEL_Y / 2,
               .size = BALL_RADIUS,
               .jump_strength = JUMP_STRENGTH,
               .velocity = 0};

  int platform_count = 2;
  Platform platforms[platform_count];
  platforms[0] =
      (Platform){.x = 0, .y = PIXEL_Y - 20, .width = PIXEL_X, .height = 20};
  platforms[1] =
      (Platform){.x = 0, .y = PIXEL_Y - 200, .width = 400, .height = 20};

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    for (int i = 0; i < platform_count; i++) {
      DrawRectangle(platforms[i].x, platforms[i].y, platforms[i].width,
                    platforms[i].height, BLACK);
    }

    // Control horizontal movement
    if (IsKeyDown(KEY_LEFT)) {
      ball.x = MAX(BALL_LEFT_LIMIT, ball.x - 8);
    }
    if (IsKeyDown(KEY_RIGHT)) {
      ball.x = MIN(BALL_RIGHT_LIMIT, ball.x + 8);
    }

    // Control vertical movement
    if ((ball_on_platform(&ball, platforms, platform_count)) &&
        IsKeyPressed(KEY_SPACE)) {
      ball.velocity = JUMP_STRENGTH;
    }
    update_vertical_location(&ball, platforms, platform_count);

    DrawCircle(ball.x, ball.y, ball.size, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
