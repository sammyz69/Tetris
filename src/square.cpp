#include "square.h"
#include "globals.h"
#include <iostream>
#include <glad/glad.h>

bool Square::get_collision(){
    return collision;
}


void Square::check_collision(){
    collision = false;
    for(int i=0; i<4; i++){
        int c = l[i] - left_wall;
        if(c < 0 || c >= 10) continue;
        if(d[i] - 1 < floors[c]){
            collision = true;
            return;
        }
    }
}

void Square::lock_piece(){
    int col_max[10];
    for(int c=0;c<10;c++) col_max[c] = -1;

    for(int i=0; i<4; i++){
        int c = l[i] - left_wall;
        if(c < 0 || c >= 10) continue;
        if(d[i] < 0 || d[i] >= 20) continue;
        heights[c][d[i]] = true;
        if(d[i] > col_max[c]) col_max[c] = d[i];
    }
    for(int c=0; c<10; c++){
        if(col_max[c] >= 0 && col_max[c] + 1 > floors[c]){
            floors[c] = col_max[c] + 1;
        }
    }

    for(int row=0; row<20; row++){
        bool full = true;
        for(int c=0; c<10; c++){
            if(!heights[c][row]){ full = false; break; }
        }
        if(full){
            for(int c=0; c<10; c++){
                for(int r=row; r<19; r++){
                    heights[c][r] = heights[c][r+1];
                }
                heights[c][19] = false;
                if(floors[c] > 0) floors[c]--;
            }
            row--; // re-check this row index, since everything above just shifted into it
        }
    }
}

void Square::gdown(){
    check_collision();
    if(!collision){
        for(int i=0; i<4; i++) {
            d[i]--;
        }
        update();
    } else {
        lock_piece();
    }
}

// FIX: gright/gleft now also check whether the destination column is
// already occupied at this piece's height (using floors[]), not just the
// outer walls — previously a piece could slide sideways INTO the stack.
void Square::gright(){
    bool can_move = true;
    for(int i=0; i<4; i++){
        if(l[i] + 1 >= right_wall){ can_move = false; break; }
        int c = (l[i] + 1) - left_wall;
        if(c >= 0 && c < 10 && d[i] < floors[c]){ can_move = false; break; }
    }
    if(can_move){
        for(int i=0; i<4; i++) l[i]++;
        update();
    }
}

void Square::gleft(){
    bool can_move = true;
    for(int i=0; i<4; i++){
        if(l[i] - 1 <= left_wall){ can_move = false; break; }
        int c = (l[i] - 1) - left_wall;
        if(c >= 0 && c < 10 && d[i] < floors[c]){ can_move = false; break; }
    }
    if(can_move){
        for(int i=0; i<4; i++) l[i]--;
        update();
    }
}


void Square::instant_down(){
    check_collision();
    while(!collision){
        for(int i=0; i<4; i++) d[i]--;
        check_collision();
    }
    update();
    lock_piece();
}

void Square::rotate(){
    check_collision();
    if(!collision){
        upd_rot_state();
        update();
    }
}

void Rect::set_rect_coords(int a, int b, int c, float width, float height){
    left_x = a;
    down_y = b;
    col = c;

    float m = static_cast<float>(a) / width_boxes;
    float n = static_cast<float>(b) / height_boxes;
    m *= 2; 
    n *= 2; 
    m--; 
    n--;

    VBOs.resize(2);

    vertices.push_back(m);            
    vertices.push_back(n);              
    vertices.push_back(0);

    vertices.push_back(m+width);   
    vertices.push_back(n);              
    vertices.push_back(0);

    vertices.push_back(m);   
    vertices.push_back(n+height);    
    vertices.push_back(0);


    vertices.push_back(m+width);   
    vertices.push_back(n+height);    
    vertices.push_back(0);

    unsigned int base = static_cast<unsigned int>(vertices.size() / 3) - 4;
    elements.push_back(base + 0);
    elements.push_back(base + 1);
    elements.push_back(base + 2);
    elements.push_back(base + 2);
    elements.push_back(base + 1);
    elements.push_back(base + 3);

    up_to_gpu(c);
    vertices.resize(0); 
    elements.resize(0);
}

void Square::set_square_coords(int a, int b, int c){
    set_rect_coords(a, b, c, width_sq, height_sq);
}

void Square::update(){
    vertices.clear();
    elements.clear();
    for(int i=0; i<4; i++){
        set_square_coords(l[i], d[i], col);
    }
    up_to_gpu(col);
}

void Rect::up_to_gpu(int c){
    unsigned int vertex_total = static_cast<unsigned int>(vertices.size() / 3);
    std::vector<int> color(vertex_total, c);

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
        glDeleteBuffers(1, &EBO);
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

float Square::set_x_coord(int a){
    float m = static_cast<float>(a) / width_boxes;
    m *= 2; 
    m--;
    return m;
}

float Square::set_y_coord(int b){
    float n = static_cast<float>(b) / height_boxes;
    n *= 2; 
    n--;
    return n;
}

int Grid::getx(){ return left_x; }
int Grid::gety(){ return down_y; }
int Grid::getcol(){ return col; }
Grid::Grid(float a, float b){}

Grid Grid::x_y(float a, float b){
    Grid temp;
    a = (a+1)/2;
    b = (b+1)/2;
    temp.left_x = a * width_boxes;
    temp.down_y = b * height_boxes;
    return temp;
}

void Grid::initialize(){
    for(float a = -1; a <= 1.00; a = a + width_sq){
        for(float b = -1; b <= 1.00; b = b + height_sq){
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

void Grid::grid_draw(){
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, vertex_count);
    glBindVertexArray(0);
}

bool Square::is_hovered(){
    int c = cursor_xpos - left_wall;
    if(c < 0 || c >= 10) return false;
    for(int i=0; i<4; i++){
        if(l[i] == cursor_xpos && d[i] == cursor_ypos) return true;
    }
    return false;
}

void Rect::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


sq_block::sq_block(int a, int b, int c){
    //2 3
    //0 1
    l[0]=a; d[0]=b; l[1]=a+1; d[1]=b; l[2]=a; d[2]=b+1; l[3]=a+1; d[3]=b+1;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);
    vertices.resize(0); elements.resize(0);
}
void sq_block::upd_rot_state(){}

l_block::l_block(int a, int b, int c){
    //3
    //2
    //0 1
    l[0]=a; d[0]=b; l[1]=a+1; d[1]=b; l[2]=a; d[2]=b+1; l[3]=a; d[3]=b+2;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);
    vertices.resize(0); elements.resize(0);
}
void l_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;
    if(rotation_state == 0){
        l[2]--; d[0]--; l[1]++; d[3]++; l[3] -= 2;
    } else if(rotation_state == 1){
        d[3] -= 2; l[3]--; d[2]--; l[0]++; d[1]++;
    } else if(rotation_state == 2){
        d[0]++; l[2]++; l[3] += 2; d[3]--; l[1]--;
    } else if(rotation_state == 3){
        d[1]--; l[0]--; d[2]++; d[3] += 2; l[3]++;
    }
}

rl_block::rl_block(int a, int b, int c){
    // - 3
    // - 2
    // 0 1
    l[0]=a; d[0]=b; l[1]=a+1; d[1]=b; l[2]=a+1; d[2]=b+1; l[3]=a+1; d[3]=b+2;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);
    vertices.resize(0); elements.resize(0);
}
void rl_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;
    if(rotation_state == 1){
        l[0]+=2; l[1]++; d[1]++; l[3]--; d[3]--;
    } else if(rotation_state == 2){
        d[0]+=2; l[1]--; d[1]++; l[3]++; d[3]--;
    } else if(rotation_state == 3){
        l[0]-=2; l[1]--; d[1]--; l[3]++; d[3]++;
    } else if(rotation_state == 0){
        d[0]-=2; l[1]++; d[1]--; l[3]--; d[3]++;
    }
}

long_block::long_block(int a, int b, int c){
    // 3
    // 2
    // 1
    // 0
    l[0]=a; d[0]=b; l[1]=a; d[1]=b+1; l[2]=a; d[2]=b+2; l[3]=a; d[3]=b+3;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);  
    vertices.resize(0); elements.resize(0);
}
void long_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;
    if(rotation_state == 1){
        l[0]++; d[0]++; l[2]--; d[2]--; l[3]-=2; d[3]-=2;
    } else if(rotation_state == 2){
        l[0]--; d[0]++; l[2]++; d[2]--; l[3]+=2; d[3]-=2;
    } else if(rotation_state == 3){
        l[0]--; d[0]--; l[2]++; d[2]++; l[3]+=2; d[3]+=2;
    } else if(rotation_state == 0){
        l[0]++; d[0]--; l[2]--; d[2]++; l[3]-=2; d[3]+=2;
    }
}

z_block::z_block(int a, int b, int c){
    // 3 2
    // - 0 1
    l[0]=a+1; d[0]=b; l[1]=a+2; d[1]=b; l[2]=a+1; d[2]=b+1; l[3]=a; d[3]=b+1;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);
    vertices.resize(0); elements.resize(0);
}
void z_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;
    if(rotation_state == 1){
        l[0]++; d[0]++; d[1]+=2; l[3]++; d[3]--;
    } else if(rotation_state == 2){
        l[0]--; d[0]++; l[1]-=2; l[3]++; d[3]++;
    } else if(rotation_state == 3){
        l[0]--; d[0]--; d[1]-=2; l[3]--; d[3]++;
    } else if(rotation_state == 0){
        l[0]++; d[0]--; l[1]+=2; l[3]--; d[3]--;
    }
}

rz_block::rz_block(int a, int b, int c){
    // - 2 3
    // 0 1
    l[0]=a; d[0]=b; l[1]=a+1; d[1]=b; l[2]=a+1; d[2]=b+1; l[3]=a+2; d[3]=b+1;
    for(int i=3;i>-1;i--) set_square_coords(l[i],d[i],c);
    vertices.resize(0); elements.resize(0);
}
void rz_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;
    if(rotation_state == 1){
        l[0]+=2; l[1]++; d[1]++; l[3]--; d[3]++;
    } else if(rotation_state == 2){
        d[0]+=2; l[1]--; d[1]++; l[3]--; d[3]--;
    } else if(rotation_state == 3){
        l[0]-=2; l[1]--; d[1]--; l[3]++; d[3]--;
    } else if(rotation_state == 0){
        d[0]-=2; l[1]++; d[1]--; l[3]++; d[3]++;
    }
}