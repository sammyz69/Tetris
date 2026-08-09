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


class Square:public Grid{
    protected:
        std::vector<unsigned int> VBOs;
        bool collision = false;
        unsigned int EBO = 0;
        unsigned int element_count = 0;
        std::vector<unsigned int> elements;

    public:
        virtual void draw();
        float set_x_coord(int x);
        float set_y_coord(int x);
        void set_square_coords(int x, int y, int c);
        virtual void gdown()=0;
        void up_to_gpu(int);
        virtual void gright()=0;
        virtual void gleft()=0;
        virtual void instant_down()=0;
        virtual void check_collision(int x)=0;
        void add_vertices(int a, int b, int c);
        virtual void update() = 0;
        virtual void rotate() = 0;
        int rotation_state = 0;
        bool get_collision();
};

class sq_block:public Square{
    protected:
        int l[4],d[4];
    public:
        sq_block(){}
        sq_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
};

class l_block:public Square{
    protected:
        int l[4],d[4];
    public:
        l_block(){}
        l_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
        void upd_rot_state();
};

class rl_block:public Square{
    protected:
        int l[4],d[4];
    public:
        rl_block(){}
        rl_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
        void upd_rot_state();
};

class long_block:public Square{
    protected:
        int l[4],d[4];
    public:
        long_block(){}
        long_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
        void upd_rot_state();
};

class z_block:public Square{
    protected:
        int l[4],d[4];
    public:
        z_block(){}
        z_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
        void upd_rot_state();
};

class rz_block:public Square{
    protected:
        int l[4],d[4];
    public:
        rz_block(){}
        rz_block(int x, int y, int z);
        void draw();
        void gdown();
        void gright();
        void gleft();
        void instant_down();
        void check_collision(int x);
        void update();
        void rotate();
        void upd_rot_state();
};