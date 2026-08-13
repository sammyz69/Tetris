#include "square.h"
#include "globals.h"
#include <iostream>
#include <glad/glad.h>

bool Square::get_collision() {
    return collision;
}

bool Square::is_spawn_position_free() const {
    int cols = right_wall - left_wall;
    for (int i = 0; i < 4; ++i) {
        int c = l[i] - left_wall;
        if (c < 0 || c >= cols) return false;
        if (d[i] < 0 || d[i] >= height_boxes) return false;
        if (board[c][d[i]] >= 0) return false;
    }
    return true;
}

void Square::check_collision() {
    collision = false;
    int cols = right_wall - left_wall;
    for (int i = 0; i < 4; i++) {
        int c = l[i] - left_wall;
        if (c < 0 || c >= cols) {
            collision = true;
            return;
        }
        int ty = d[i] - 1;
        if (ty < 0 || board[c][ty] >= 0) {
            collision = true;
            return;
        }
    }
}

void Square::lock_piece() {
    int cols = right_wall - left_wall;
    int col_max[MAX_COLS];
    for (int c = 0; c < cols; c++) col_max[c] = -1;

    for (int i = 0; i < 4; i++) {
        int c = l[i] - left_wall;
        if (c < 0 || c >= cols) continue;
        if (d[i] < 0 || d[i] >= height_boxes) continue;
        board[c][d[i]] = col;
        if (d[i] > col_max[c]) col_max[c] = d[i];
    }
    for (int c = 0; c < cols; c++) {
        if (col_max[c] >= 0 && col_max[c] + 1 > floors[c]) {
            floors[c] = col_max[c] + 1;
        }
    }

    int lines_cleared = 0;
    for (int row = 0; row < height_boxes; row++) {
        bool full = true;
        for (int c = 0; c < cols; c++) {
            if (board[c][row] < 0) { full = false; break; }
        }
        if (full) {
            lines_cleared++;
            for (int c = 0; c < cols; c++) {
                for (int r = row; r < height_boxes - 1; r++) {
                    board[c][r] = board[c][r + 1];
                }
                board[c][height_boxes - 1] = -1;
                if (floors[c] > 0) floors[c]--;
            }
            row--;
        }
    }

    if (lines_cleared == 1) user_score += 100;
    else if (lines_cleared == 2) user_score += 300;
    else if (lines_cleared == 3) user_score += 500;
    else if (lines_cleared >= 4) user_score += 800;
}

void Square::gdown() {
    check_collision();
    if (!collision) {
        for (int i = 0; i < 4; i++) 
        {d[i]--;}
        update();
    }
}

void Square::gright() {
    bool can_move = true;
    int cols = right_wall - left_wall;
    for (int i = 0; i < 4; i++) {
        if (l[i] + 1 >= right_wall) { can_move = false; break; }
        int c = (l[i] + 1) - left_wall;
        if (c >= 0 && c < cols) {
            if (d[i] >= 0 && d[i] < height_boxes && board[c][d[i]] >= 0) {
                can_move = false;
                break;
            }
        }
    }
    if (can_move) {
        for (int i = 0; i < 4; i++) l[i]++;
        update();
    }
}

void Square::gleft() {
    bool can_move = true;
    int cols = right_wall - left_wall;
    for (int i = 0; i < 4; i++) {
        if (l[i] - 1 < left_wall) { can_move = false; break; }
        int c = (l[i] - 1) - left_wall;
        if (c >= 0 && c < cols) {
            if (d[i] >= 0 && d[i] < height_boxes && board[c][d[i]] >= 0) {
                can_move = false;
                break;
            }
        }
    }
    if (can_move) {
        for (int i = 0; i < 4; i++) l[i]--;
        update();
    }
}

void Square::instant_down() {
    while (true) {
        bool would_collide = false;
        int cols = right_wall - left_wall;
        for (int i = 0; i < 4; i++) {
            int c = l[i] - left_wall;
            int ty = d[i] - 1;
            if (c < 0 || c >= cols || ty < 0 || board[c][ty] >= 0) {
                would_collide = true;
                break;
            }
        }
        if (would_collide) break;
        for (int i = 0; i < 4; i++) d[i]--;
    }
    update();
    collision = true;
}

void Square::rotate() {
    for (int i = 0; i < 4; i++) {
        lastx[i] = l[i];
        lasty[i] = d[i];
    }

    upd_rot_state();
    int shift = 0;
    for (int i = 0; i < 4; i++) {
        if (left_wall - l[i] > shift) shift = left_wall - l[i];
    }
    if (shift > 0) {
        for (int i = 0; i < 4; i++) l[i] += shift;
    } else {
        for (int i = 0; i < 4; i++) {
            if (l[i] - (right_wall - 1) > shift) shift = l[i] - (right_wall - 1);
        }
        if (shift > 0) {
            for (int i = 0; i < 4; i++) l[i] -= shift;
        }
    }
    update();
}

void Rect::set_rect_coords(int a, int b, int c, float width, float height, bool sq) {
    left_x = a;
    down_y = b;
    col = c;

    float m = static_cast<float>(a) / width_boxes;
    float n = static_cast<float>(b) / height_boxes;
    m *= 2; n *= 2; m--; n--;
    float w = 0, h = 0;
    VBOs.resize(2);
        w = border_width;
        h = border_height;
    vertices.push_back(m);           vertices.push_back(n + h);           vertices.push_back(0);
    vertices.push_back(m + width - w);   vertices.push_back(n + h);           vertices.push_back(0);
    vertices.push_back(m);           vertices.push_back(n + height);  vertices.push_back(0);
    vertices.push_back(m + width - w);   vertices.push_back(n + height);  vertices.push_back(0);

    unsigned int base = static_cast<unsigned int>(vertices.size() / 3) - 4;
    elements.push_back(base + 0);
    elements.push_back(base + 1);
    elements.push_back(base + 2);
    elements.push_back(base + 2);
    elements.push_back(base + 1);
    elements.push_back(base + 3);

    if (!sq) {
        up_to_gpu(c);
        vertices.resize(0);
        elements.resize(0);
    }
}

void Square::set_square_coords(int a, int b, int c) {
    set_rect_coords(a, b, c, width_sq, height_sq, true);
}

void Square::update() {
    vertices.clear();
    elements.clear();
    for (int i = 0; i < 4; i++) {
        set_square_coords(l[i], d[i], col);
    }
    up_to_gpu(col);
}

void Rect::up_to_gpu(int c) {
    unsigned int vertex_total = static_cast<unsigned int>(vertices.size() / 3);
    std::vector<int> color(vertex_total, c);

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        if (!VBOs.empty())
            glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);
        VAO = 0;
        EBO = 0;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(int), color.data(), GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void*)0);
    glEnableVertexAttribArray(1);

    element_count = static_cast<unsigned int>(elements.size());
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

float Square::set_x_coord(int a) {
    float m = static_cast<float>(a) / width_boxes;
    m *= 2; m--;
    return m;
}

float Square::set_y_coord(int b) {
    float n = static_cast<float>(b) / height_boxes;
    n *= 2; n--;
    return n;
}

int Grid::getx() { return left_x; }
int Grid::gety() { return down_y; }
int Grid::getcol() { return col; }
Grid::Grid(float a, float b) {}

Grid Grid::x_y(float a, float b) {
    Grid temp;
    a = (a + 1) / 2;
    b = (b + 1) / 2;
    temp.left_x = a * width_boxes;
    temp.down_y = b * height_boxes;
    return temp;
}

void Grid::initialize() {
    for (float a = -1; a <= 1.00; a = a + width_sq) {
        for (float b = -1; b <= 1.00; b = b + height_sq) {
            vertices.push_back(a);
            vertices.push_back(b);
            vertices.push_back(0);
            vertex_count += 3;
        }
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Grid::grid_draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, vertex_count);
    glBindVertexArray(0);
}

bool Square::is_hovered() {
    int cols = right_wall - left_wall;
    int c = cursor_xpos - left_wall;
    if (c < 0 || c >= cols) return false;
    for (int i = 0; i < 4; i++) {
        if (l[i] == cursor_xpos && d[i] == cursor_ypos) return true;
    }
    return false;
}

void Rect::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// -------------------- constructors & rotations --------------------

sq_block::sq_block(int a, int b, int c) {
    col = c;
    l[0] = a;     d[0] = b;
    l[1] = a + 1; d[1] = b;
    l[2] = a;     d[2] = b + 1;
    l[3] = a + 1; d[3] = b + 1;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void sq_block::upd_rot_state() {}

l_block::l_block(int a, int b, int c) {
    col = c;
    l[0] = a;     d[0] = b;
    l[1] = a + 1; d[1] = b;
    l[2] = a;     d[2] = b + 1;
    l[3] = a;     d[3] = b + 2;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void l_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 0) {
        l[2]--; d[0]--; l[1]++; d[3]++; l[3] -= 2;
    } else if (rotation_state == 1) {
        d[3] -= 2; l[3]--; d[2]--; l[0]++; d[1]++;
    } else if (rotation_state == 2) {
        d[0]++; l[2]++; l[3] += 2; d[3]--; l[1]--;
    } else if (rotation_state == 3) {
        d[1]--; l[0]--; d[2]++; d[3] += 2; l[3]++;
    }
}

rl_block::rl_block(int a, int b, int c) {
    col = c;
    l[0] = a;     d[0] = b;
    l[1] = a + 1; d[1] = b;
    l[2] = a + 1; d[2] = b + 1;
    l[3] = a + 1; d[3] = b + 2;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void rl_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 1) {
        l[0] += 2; l[1]++; d[1]++; l[3]--; d[3]--;
    } else if (rotation_state == 2) {
        d[0] += 2; l[1]--; d[1]++; l[3]++; d[3]--;
    } else if (rotation_state == 3) {
        l[0] -= 2; l[1]--; d[1]--; l[3]++; d[3]++;
    } else if (rotation_state == 0) {
        d[0] -= 2; l[1]++; d[1]--; l[3]--; d[3]++;
    }
}

long_block::long_block(int a, int b, int c) {
    col = c;
    l[0] = a; d[0] = b;
    l[1] = a; d[1] = b + 1;
    l[2] = a; d[2] = b + 2;
    l[3] = a; d[3] = b + 3;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void long_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 1) {
        l[0]++; d[0]++; l[2]--; d[2]--; l[3] -= 2; d[3] -= 2;
    } else if (rotation_state == 2) {
        l[0]--; d[0]++; l[2]++; d[2]--; l[3] += 2; d[3] -= 2;
    } else if (rotation_state == 3) {
        l[0]--; d[0]--; l[2]++; d[2]++; l[3] += 2; d[3] += 2;
    } else if (rotation_state == 0) {
        l[0]++; d[0]--; l[2]--; d[2]++; l[3] -= 2; d[3] += 2;
    }
}

z_block::z_block(int a, int b, int c) {
    col = c;
    l[0] = a + 1; d[0] = b;
    l[1] = a + 2; d[1] = b;
    l[2] = a + 1; d[2] = b + 1;
    l[3] = a;     d[3] = b + 1;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void z_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 1) {
        l[0]++; d[0]++; d[1] += 2; l[3]++; d[3]--;
    } else if (rotation_state == 2) {
        l[0]--; d[0]++; l[1] -= 2; l[3]++; d[3]++;
    } else if (rotation_state == 3) {
        l[0]--; d[0]--; d[1] -= 2; l[3]--; d[3]++;
    } else if (rotation_state == 0) {
        l[0]++; d[0]--; l[1] += 2; l[3]--; d[3]--;
    }
}

rz_block::rz_block(int a, int b, int c) {
    col = c;
    l[0] = a;     d[0] = b;
    l[1] = a + 1; d[1] = b;
    l[2] = a + 1; d[2] = b + 1;
    l[3] = a + 2; d[3] = b + 1;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void rz_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 1) {
        l[0] += 2; l[1]++; d[1]++; l[3]--; d[3]++;
    } else if (rotation_state == 2) {
        d[0] += 2; l[1]--; d[1]++; l[3]--; d[3]--;
    } else if (rotation_state == 3) {
        l[0] -= 2; l[1]--; d[1]--; l[3]++; d[3]--;
    } else if (rotation_state == 0) {
        d[0] -= 2; l[1]++; d[1]--; l[3]++; d[3]++;
    }
}

new_block::new_block(int a, int b, int c) {
    col = c;
    l[0] = a;     d[0] = b;
    l[1] = a + 1; d[1] = b;
    l[2] = a + 1; d[2] = b + 1;
    l[3] = a + 2; d[3] = b + 1;
    for (int i = 3; i > -1; i--) set_square_coords(l[i], d[i], c);
    vertices.resize(0); elements.resize(0);
    up_to_gpu(c);
}
void new_block::upd_rot_state() {
    rotation_state = (rotation_state + 1) % 4;
    if (rotation_state == 1) {
        l[0]++; d[0]--; l[3]--; d[3]++;
    } else if (rotation_state == 2) {
        l[0]--; d[0]++; l[3]--; d[3]--;
    } else if (rotation_state == 3) {
        l[0]++; d[0] += 2; l[1]--; d[1]++; l[3] += 2;
    } else if (rotation_state == 0) {
        l[0]--; d[0] -= 2; l[1]++; d[1]--;
    }
}