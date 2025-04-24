#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
  // We only consider the bottom 2 pixels of the ball for collision
  Rectangle ball_base = {.x = (*ball).x - (*ball).size + 20,
                         .y = (*ball).y + (*ball).size - 1,
                         .width = (*ball).size * 2 - 20,
                         .height = 2};
  for (int i = 0; i < platform_count; i++) {
    Rectangle rectangle = *(Rectangle *)&platform[i];
    if (CheckCollisionRecs(ball_base, rectangle)) {
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
  srand(time(NULL));

  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  InitWindow(PIXEL_X, PIXEL_Y, "cgame");

  Ball ball = {.x = PIXEL_X / 2,
               0,
               .size = BALL_RADIUS,
               .jump_strength = JUMP_STRENGTH,
               .velocity = 0};

  int platform_count = 10;
  Platform platforms[platform_count];
  platforms[0] =
      (Platform){.x = 0, .y = PIXEL_Y - 20, .width = PIXEL_X, .height = 20};
  for (int i = 1; i < platform_count; i++) {
    platforms[i] = (Platform){.x = rand() % PIXEL_X - 100,
                              .y = platforms[i - 1].y - 100,
                              .width = 400,
                              .height = 20};
  }

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

    // Add jump
    if ((ball_on_platform(&ball, platforms, platform_count)) &&
        IsKeyPressed(KEY_SPACE)) {
      ball.velocity = JUMP_STRENGTH;
    }

    // Move the platforms
    // Platfrom at index 0 is the ground, do not move the ground
    // Add some randomization when platforms are reborn
    for (int i = 1; i < platform_count; i++) {
      platforms[i].y += 1;
      if (platforms[i].y > PIXEL_Y + 200) {
        platforms[i].y = 0 - platforms[i].height;
        platforms[i].x = rand() % PIXEL_X - 100;
        platforms[i].width = rand() % 400 + 200;
      }
    }

    update_vertical_location(&ball, platforms, platform_count);

    DrawCircle(ball.x, ball.y, ball.size, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
