#pragma once

// Display settings
constexpr int DISPLAY_WIDTH  = 450;
constexpr int DISPLAY_HEIGHT = 800;

// Arena settings
constexpr float ARENA_CENTER_X = DISPLAY_WIDTH / 2.0f;
constexpr float ARENA_CENTER_Y = DISPLAY_HEIGHT / 2.0f;
constexpr float ARENA_RADIUS = DISPLAY_WIDTH * 0.44f; // ~198px de margen lateral

// Ball settings
constexpr float BALL_RADIUS = 18.0f;
constexpr float INITIAL_VELOCITY_X = -1.0f;
constexpr float INITIAL_VELOCITY_Y = 1.0f;