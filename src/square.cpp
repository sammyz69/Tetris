#include "square.h"
#include "globals.h"
#include <iostream>
#include <glad/glad.h>

void sq_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

bool Square::get_collision(){
    return collision;
}

void sq_block::check_collision(int a){
    collision = false;
    for(int i=0; i<10; i++)
    {
        if(a == floors[i]){
            collision = true;
            break;
        }
    }
    
    if(collision){
        floors[l[0]-10] = d[2]+1;
        floors[l[1]-10] = d[3]+1;
    }
}

void sq_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }

}

void sq_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void sq_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void Square::set_square_coords(int a, int b, int c){

    left_x = a;
    down_y = b;
    col = c;

 //   std::cout << "  left: "  << left_x << "  down  :" << down_y << "  col   " << col << std::endl;  

    //a can range from 0 to width_boxes
    float m = static_cast<float>(a) / width_boxes;
    float n = static_cast<float>(b) / height_boxes;

    //0-1 converted to 0-2 then to -1 to 1
    m *= 2;
    n *= 2;
    m --;
    n--;


    //std::cout << " b4 resize ";
    VBOs.resize(2);
    
//std::cout << " af resize ";

    vertices.push_back(m);
    //std::cout << " m ";
    vertices.push_back(n);
   // std::cout << " n ";
    vertices.push_back(0);

 //std::cout << " b4 pushback ";

    vertices.push_back(m+width_sq);
    vertices.push_back(n);
    vertices.push_back(0);

    vertices.push_back(m);
    vertices.push_back(n+height_sq);
    vertices.push_back(0);

    vertices.push_back(m+width_sq);
    vertices.push_back(n+height_sq);
    vertices.push_back(0);

    unsigned int base = static_cast<unsigned int>(vertices.size() / 3) - 4;
    elements.push_back(base + 0);
    elements.push_back(base + 1);
    elements.push_back(base + 2);
    elements.push_back(base + 2);
    elements.push_back(base + 1);
    elements.push_back(base + 3);
}

void sq_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}

void Square::up_to_gpu(int c){

    unsigned int vertex_total = static_cast<unsigned int>(vertices.size() / 3);
    std::vector<int> color(vertex_total, c);

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
        glDeleteBuffers(1, &EBO);
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

   // std::cout << " b4 gen buffers ";
    glGenBuffers(1, &VBOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

 //std::cout << " b4 color ";
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
    //a can range from 0 to width_boxes
    float m = static_cast<float>(a) / width_boxes;


    //0-1 converted to 0-2 then to -1 to 1
    m *= 2;
    m --;

    return m;    
}

float Square::set_y_coord(int b){
    //a can range from 0 to width_boxes
    float n = static_cast<float>(b) / height_boxes;

    //0-1 converted to 0-2 then to -1 to 1

    n *= 2;
    n--;

    return n;
}

int Grid::getx(){
    return left_x;
}

int Grid::gety(){
    return down_y;
}

int Grid::getcol(){
    return col;
}

Grid::Grid(float a, float b){
    
}

Grid Grid::x_y(float a, float b){
    Grid temp;


    //normalize to 0-1 coordinates and then to a 30x20 grid
    a = (a+1)/2;
    b = (b+1)/2;
    temp.left_x = a * width_boxes;
    temp.down_y = b * height_boxes;
    
    return temp;
}

void Grid::initialize(){
    for(float a = -1; a <= 1.00; a = a + width_sq)
    {
        for(float b = -1; b <= 1.00; b = b + height_sq)
        {
            //a, b, 0 for positions
            vertices.push_back(a);
            vertices.push_back(b);
            vertices.push_back(0);
            vertex_count +=3;
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
    glDrawArrays(GL_POINTS, 0 , vertex_count);
    glBindVertexArray(0);
}

void Square::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void sq_block::draw(){

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

sq_block::sq_block(int a, int b, int c){
    //define the four left and down corners
    //a and b define the leftmost and downmost corners

    //2 3
    //0 1
    l[0] = a;
    d[0] = b;
    l[1] = a + 1;
    d[1] = b;
    l[2] = a;
    d[2] = b + 1;
    l[3] = a + 1;
    d[3] = b + 1;

    for(int i=3; i>-1; i--)
    {
  //      std::cout << "   i value:  " << i  << "  l[i]:  "<< l[i] << std::endl;
        set_square_coords(l[i],d[i],c);
 //       std:: cout << "  loop check   ";
    }
    up_to_gpu(c);
    vertices.resize(0);

    elements.resize(0);
}

void sq_block::rotate(){
    update();
}

void l_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

void l_block::check_collision(int a){
    if(a != 0)
    {
        collision = false;
    }
    else{
        collision = true;
    }
}

void l_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }
}

void l_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void l_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void l_block::draw(){

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

l_block::l_block(int a, int b, int c){
    //define the four left and down corners
    //a and b define the leftmost and downmost corners

    //3
    //2
    //0 1
    l[0] = a;
    d[0] = b;
    l[1] = a + 1;
    d[1] = b;
    l[2] = a;
    d[2] = b + 1;
    l[3] = a;
    d[3] = b + 2;

    for(int i=3; i>-1; i--)
    {
  //      std::cout << "   i value:  " << i  << "  l[i]:  "<< l[i] << std::endl;
        set_square_coords(l[i],d[i],c);
 //       std:: cout << "  loop check   ";
    }
    up_to_gpu(c);
    vertices.resize(0);

    elements.resize(0);
}

void l_block::rotate(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        upd_rot_state();
        update();
    }
}

void l_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;

        if(rotation_state == 0){
                l[2]--;
                d[0]--;
                l[1]++;
                d[3]++;
                l[3] = l[3] -2;
                std::cout << "fdaslkfas";
            }
        else if(rotation_state == 1){
                d[3]--;
                d[3]--;
                l[3]--;
                d[2]--;
                l[0]++;
                d[1]++;
            }
        else if(rotation_state == 2){
                d[0]++;
                l[2]++;
                l[3] = l[3] + 2;
                d[3]--;
                l[1]--;
            }   
        else if(rotation_state == 3){
                d[1]--;
                l[0]--;
                d[2]++;
                d[3] = d[3] +2;
                l[3]++;
            }
    update();
}

void l_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}

// ===================== RL_BLOCK =====================
// - 3
// - 2
// 0 1
void rl_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

void rl_block::check_collision(int a){
    if(a != 0)
    {
        collision = false;
    }
    else{
        collision = true;
    }
}

void rl_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }
}

void rl_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void rl_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void rl_block::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

rl_block::rl_block(int a, int b, int c){
    // - 3
    // - 2
    // 0 1
    l[0] = a;
    d[0] = b;
    l[1] = a + 1;
    d[1] = b;
    l[2] = a + 1;
    d[2] = b + 1;
    l[3] = a + 1;
    d[3] = b + 2;

    for(int i=3; i>-1; i--)
    {
        set_square_coords(l[i],d[i],c);
    }
    up_to_gpu(c);
    vertices.resize(0);
    elements.resize(0);
}

void rl_block::rotate(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        upd_rot_state();
        update();
    }
}

void rl_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;

        if(rotation_state == 1){
                l[0]++;
                l[0]++;
                l[1]++;
                d[1]++;
                l[3]--;
                d[3]--;
            }
        else if(rotation_state == 2){
                d[0]++;
                d[0]++;
                l[1]--;
                d[1]++;
                l[3]++;
                d[3]--;
            }
        else if(rotation_state == 3){
                l[0]--;
                l[0]--;
                l[1]--;
                d[1]--;
                l[3]++;
                d[3]++;
            }   
        else if(rotation_state == 0){
                d[0]--;
                d[0]--;
                l[1]++;
                d[1]--;
                l[3]--;
                d[3]++;
            }
    update();
}

void rl_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}


// ===================== LONG_BLOCK =====================
// 3
// 2
// 1
// 0
void long_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

void long_block::check_collision(int a){
    if(a != 0)
    {
        collision = false;
    }
    else{
        collision = true;
    }
}

void long_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }
}

void long_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void long_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void long_block::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

long_block::long_block(int a, int b, int c){
    // 3
    // 2
    // 1
    // 0
    l[0] = a;
    d[0] = b;
    l[1] = a;
    d[1] = b + 1;
    l[2] = a;
    d[2] = b + 2;
    l[3] = a;
    d[3] = b + 3;

    for(int i=3; i>-1; i--)
    {
        set_square_coords(l[i],d[i],c);
    }
    up_to_gpu(c);
    vertices.resize(0);
    elements.resize(0);
}

void long_block::rotate(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        upd_rot_state();
        update();
    }
}

void long_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;

        if(rotation_state == 1){
                l[0]++;
                d[0]++;
                l[2]--;
                d[2]--;
                l[3]--;
                l[3]--;
                d[3]--;
                d[3]--;
            }
        else if(rotation_state == 2){
                l[0]--;
                d[0]++;
                l[2]++;
                d[2]--;
                l[3]++;
                l[3]++;
                d[3]--;
                d[3]--;
            }
        else if(rotation_state == 3){
                l[0]--;
                d[0]--;
                l[2]++;
                d[2]++;
                l[3]++;
                l[3]++;
                d[3]++;
                d[3]++;
            }   
        else if(rotation_state == 0){
                l[0]++;
                d[0]--;
                l[2]--;
                d[2]++;
                l[3]--;
                l[3]--;
                d[3]++;
                d[3]++;
            }
    update();
}

void long_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}


// ===================== Z_BLOCK =====================
// 3 2
// - 0 1
void z_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

void z_block::check_collision(int a){
    if(a != 0)
    {
        collision = false;
    }
    else{
        collision = true;
    }
}

void z_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }
}

void z_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void z_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void z_block::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

z_block::z_block(int a, int b, int c){
    // 3 2
    // - 0 1
    l[0] = a + 1;
    d[0] = b;
    l[1] = a + 2;
    d[1] = b;
    l[2] = a + 1;
    d[2] = b + 1;
    l[3] = a;
    d[3] = b + 1;

    for(int i=3; i>-1; i--)
    {
        set_square_coords(l[i],d[i],c);
    }
    up_to_gpu(c);
    vertices.resize(0);
    elements.resize(0);
}

void z_block::rotate(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        upd_rot_state();
        update();
    }
}

void z_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;

        if(rotation_state == 1){
                l[0]++;
                d[0]++;
                d[1]++;
                d[1]++;
                l[3]++;
                d[3]--;
            }
        else if(rotation_state == 2){
                l[0]--;
                d[0]++;
                l[1]--;
                l[1]--;
                l[3]++;
                d[3]++;
            }
        else if(rotation_state == 3){
                l[0]--;
                d[0]--;
                d[1]--;
                d[1]--;
                l[3]--;
                d[3]++;
            }   
        else if(rotation_state == 0){
                l[0]++;
                d[0]--;
                l[1]++;
                l[1]++;
                l[3]--;
                d[3]--;
            }
    update();
}

void z_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}


// ===================== RZ_BLOCK =====================
// - 2 3
// 0 1
void rz_block::instant_down(){
    for(int i=0; i<25; i++)
    {
        gdown();
    }
}

void rz_block::check_collision(int a){
    if(a != 0)
    {
        collision = false;
    }
    else{
        collision = true;
    }
}

void rz_block::gdown(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        for(int i=0; i<4; i++)
        {
            d[i]--;
        }
        update();
    }
}

void rz_block::gright(){
    bool can_go_right = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==right_wall || collision){
            can_go_right = false;
            break;
        }
    }
    if(can_go_right){
        for(int i=0;i<4;i++)
        {
            l[i]++;
            update();
        }
    }
}

void rz_block::gleft(){
    bool can_go_left = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(l[i]==left_wall || collision){
            can_go_left = false;
            break;
        }
    }
    if(can_go_left){
        for(int i=0;i<4;i++)
        {
            l[i]--;
            update();
        }
    }
}

void rz_block::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

rz_block::rz_block(int a, int b, int c){
    // - 2 3
    // 0 1
    l[0] = a;
    d[0] = b;
    l[1] = a + 1;
    d[1] = b;
    l[2] = a + 1;
    d[2] = b + 1;
    l[3] = a + 2;
    d[3] = b + 1;

    for(int i=3; i>-1; i--)
    {
        set_square_coords(l[i],d[i],c);
    }
    up_to_gpu(c);
    vertices.resize(0);
    elements.resize(0);
}

void rz_block::rotate(){
    bool can_move = true;
    for(int i=0; i<4; i++)
    {
        check_collision(d[i]);
        if(collision){
            can_move = false;
            break;
        }
    }
    if(can_move)
    {
        upd_rot_state();
        update();
    }
}

void rz_block::upd_rot_state(){
    rotation_state = (rotation_state + 1) % 4;

        if(rotation_state == 1){
                l[0]++;
                l[0]++;
                l[1]++;
                d[1]++;
                l[3]--;
                d[3]++;
            }
        else if(rotation_state == 2){
                d[0]++;
                d[0]++;
                l[1]--;
                d[1]++;
                l[3]--;
                d[3]--;
            }
        else if(rotation_state == 3){
                l[0]--;
                l[0]--;
                l[1]--;
                d[1]--;
                l[3]++;
                d[3]--;
            }   
        else if(rotation_state == 0){
                d[0]--;
                d[0]--;
                l[1]++;
                d[1]--;
                l[3]++;
                d[3]++;
            }
    update();
}

void rz_block::update(){
    vertices.clear();
    elements.clear();
    for(int i=0;i<4;i++){
        set_square_coords(l[i],d[i],col);
    }
    up_to_gpu(col);
}