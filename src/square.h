#pragma once
#include <vector>
#include <iostream>
#include <glad/glad.h>

class Grid {
protected:
    int left_x = 0, down_y = 0, col = 0;
    std::vector<float> vertices;
    unsigned int vertex_count = 0, VAO = 0, VBO = 0;
    int high[10] = {0};
public:
    Grid() {}
    Grid(float a, float b);
    Grid x_y(float a, float b);
    void set_float_coords(int a, int b);
    void initialize();
    void grid_draw();
    int getx();
    int gety();
    int getcol();
};

class Rect : public Grid {
protected:
    unsigned int EBO = 0;
    unsigned int element_count = 0;
    std::vector<unsigned int> elements;
    std::vector<unsigned int> VBOs;
public:
    Rect() {}
    ~Rect() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            if (!VBOs.empty())
                glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
            if (EBO != 0)
                glDeleteBuffers(1, &EBO);
            VAO = 0;
            EBO = 0;
            VBOs.clear();
        }
    }

    void set_rect_coords(int a, int b, int c, float size_width, float size_height, bool square);
    void draw();
    void up_to_gpu(int);
    int ret() { return element_count; }
};

class Square : public Rect {
protected:
    bool collision = false;
    int l[4], d[4];
    int lastx[4], lasty[4];

public:
    float set_x_coord(int x);
    float set_y_coord(int x);
    void set_square_coords(int x, int y, int c);

    void gdown();
    void gright();
    void gleft();
    void instant_down();
    void check_collision();
    void update();
    void rotate();
    bool is_hovered();
    void lock_piece();

    // Returns true if the 4 cells the piece currently occupies are empty
    bool is_spawn_position_free() const;

    virtual void upd_rot_state() = 0;
    int rotation_state = 0;
    bool get_collision();
};

class sq_block : public Square {
public:
    sq_block() {}
    sq_block(int x, int y, int z);
    void upd_rot_state() override;
};

class l_block : public Square {
public:
    l_block() {}
    l_block(int x, int y, int z);
    void upd_rot_state() override;
};

class rl_block : public Square {
public:
    rl_block() {}
    rl_block(int x, int y, int z);
    void upd_rot_state() override;
};

class long_block : public Square {
public:
    long_block() {}
    long_block(int x, int y, int z);
    void upd_rot_state() override;
};

class z_block : public Square {
public:
    z_block() {}
    z_block(int x, int y, int z);
    void upd_rot_state() override;
};

class rz_block : public Square {
public:
    rz_block() {}
    rz_block(int x, int y, int z);
    void upd_rot_state() override;
};

class new_block : public Square {
public:
    new_block() {}
    new_block(int x, int y, int z);
    void upd_rot_state() override;
};