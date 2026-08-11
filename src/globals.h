#pragma once

inline int screen_width = 1800;
inline int screen_height = 1200;
inline int user_score = 0;
inline int width_boxes = 30;
inline int height_boxes = 20;
inline int frame_rate = 1;

inline int down_floor = 0;
inline int left_wall = 10;
inline int right_wall = width_boxes - left_wall;


inline float width_sq = static_cast<float> (2) / width_boxes;
inline float height_sq = static_cast<float> (2) / height_boxes;
inline float distort;

inline int floors[10] = {0};
inline bool heights[10][20] = {false};

inline int screen = 0;

inline int cursor_xpos = 0;
inline int cursor_ypos = 0;

