#pragma once
#include <vector>
#include <iostream>

class Grid{
    protected:
        int left_x, down_y, col;
        std::vector<float> vertices;
        unsigned int vertex_count=0, VAO, VBO;
        int high[10] = {0};
    public:
        Grid(){}
        Grid(float a, float b);
        Grid x_y(float a, float b);
        void set_float_coords(int a, int b);
        void initialize();
        void grid_draw();
        int getx();
        int gety();
        int getcol();
};

// FIX: l[4]/d[4] moved here from every derived class — they were identical
// duplicates in sq_block, l_block, rl_block, long_block, z_block, rz_block.
// This is what lets gdown/gright/gleft/check_collision/update/draw be
// written ONCE instead of six times.

class Rect:public Grid{
    protected:
        unsigned int EBO = 0;
        unsigned int element_count = 0;
        std::vector<unsigned int> elements;
        std::vector<unsigned int> VBOs;
    public:

        Rect(){}
        void set_rect_coords(int a, int b, int c, float size_width, float size_height, bool square);
        void draw(); 
        void up_to_gpu(int);
        int ret(){ 
            return element_count; }
};
class Square:public Rect{
    protected:
        bool collision = false;
        int l[4], d[4];

    public:                    // FIX: no longer virtual/overridden per class — identical everywhere
        float set_x_coord(int x);
        float set_y_coord(int x);
        void set_square_coords(int x, int y, int c);

        // FIX: these five are now shared, single implementations — no
        // longer virtual, no longer duplicated per block type.
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision();          // FIX: no longer takes an int — checks all 4 of its own corners against their own columns
        void update();
        void rotate();    
        bool is_hovered();

        // FIX: shared — checks collision, then defers to shape-specific upd_rot_state()

        // FIX: lock_piece() is new — called once a piece can no longer
        // fall. Updates floors[]/heights[][] per-column correctly (every
        // corner, not just 2 hardcoded ones), then checks for and clears
        // any full rows.
        void lock_piece();

        virtual void upd_rot_state() = 0; // still per-shape: only real difference between block types now
        int rotation_state = 0;
        bool get_collision();
};

class sq_block:public Square{
    public:
        sq_block(){}
        sq_block(int x, int y, int z);
        void upd_rot_state(); // empty — a square looks the same at every rotation
};

class l_block:public Square{
    public:
        l_block(){}
        l_block(int x, int y, int z);
        void upd_rot_state();
};

class rl_block:public Square{
    public:
        rl_block(){}
        rl_block(int x, int y, int z);
        void upd_rot_state();
};

class long_block:public Square{
    public:
        long_block(){}
        long_block(int x, int y, int z);
        void upd_rot_state();
};

class z_block:public Square{
    public:
        z_block(){}
        z_block(int x, int y, int z);
        void upd_rot_state();
};

class rz_block:public Square{
    public:
        rz_block(){}
        rz_block(int x, int y, int z);
        void upd_rot_state();
};

class new_block:public Square{
    public:
        new_block(){}
        new_block(int x, int y, int z);
        void upd_rot_state();
};