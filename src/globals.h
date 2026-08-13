#pragma once
#include <fstream>
inline int screen_width = 1800;
inline int screen_height = 1200;
inline int user_score = 0;
inline int width_boxes = 30;
inline int height_boxes = 20;

inline int anim_frame_rate = 30;
inline int drop_rate_inverse = 20;
inline int move_rate_inverse = 13;

inline constexpr int FRAME_RATE_EASY   = 1;
inline constexpr int FRAME_RATE_MEDIUM = 3;
inline constexpr int FRAME_RATE_HARD   = 6;
inline int frame_rate = FRAME_RATE_MEDIUM;

inline int down_floor = 0;
inline int left_wall = 8;

inline int high_scores[3] = {0,0,0};
inline void load_high_scores(const char* path = "highscores.txt") {
    std::ifstream f(path);
    if (!f.is_open()) return;
    for (int i = 0; i < 3 && f >> high_scores[i]; i++) {}
}

inline void save_high_scores(const char* path = "highscores.txt") {
    std::ofstream f(path);
    if (!f.is_open()) return;
    for (int i = 0; i < 3; i++) f << high_scores[i] << "\n";
}

inline bool try_insert_high_score(int score) {
    if (score <= high_scores[2]) return false;
    high_scores[2] = score;
    if (high_scores[2] > high_scores[1]) std::swap(high_scores[2], high_scores[1]);
    if (high_scores[1] > high_scores[0]) std::swap(high_scores[1], high_scores[0]);
    if (high_scores[2] > high_scores[1]) std::swap(high_scores[2], high_scores[1]);
    save_high_scores();
    return true;
}

inline int right_wall = width_boxes - left_wall;

inline float width_sq = static_cast<float>(2) / width_boxes;
inline float height_sq = static_cast<float>(2) / height_boxes;
inline float distort;

inline constexpr int MAX_COLS = 30;
inline constexpr int MAX_ROWS = 30;
inline int floors[MAX_COLS] = {0};

inline int board[MAX_COLS][MAX_ROWS];

inline void reset_playfield(){
    for(int c=0; c<MAX_COLS; c++){
        floors[c] = 0;
        for(int r=0; r<MAX_ROWS; r++) board[c][r] = -1;
    }
}

inline void set_left_wall(int new_left_wall){
    left_wall = new_left_wall;
    right_wall = width_boxes - left_wall;
    reset_playfield();
}

inline int screen = 0;

inline int cursor_xpos = 0;
inline int cursor_ypos = 0;

inline float border_width = width_sq / 10;
inline float border_height = height_sq / 10;