#include "config.h"
#include "square.h"
#include "globals.h"
#include "material.h"


bool input_flag = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    input_flag = true;
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos){
    cursor_xpos = (xpos / screen_width) * width_boxes ;
    cursor_ypos = ((screen_height - ypos) / (screen_height)) * height_boxes;
}

int hovering(){
    //0 none
    //1 exit
    //2 high scores
    //3 settings
    //4 start
    if((cursor_xpos >=13 && cursor_xpos <=17) && (cursor_ypos == 3))
    {
        return 1;
    }
    else if((cursor_xpos >=9 && cursor_xpos <=20) && (cursor_ypos >= 5 && cursor_ypos<=6))
    {
        return 2;
    }
    if((cursor_xpos >=11 && cursor_xpos <=19) && (cursor_ypos >= 7 && cursor_ypos <=8))
    {
        return 3;
    }
    if((cursor_xpos >=12 && cursor_xpos <=18) && (cursor_ypos >= 9 && cursor_ypos <=10))
    {
        return 4;
    }
    else{
        return 0;
    }
}

unsigned int make_module(const string& filepath, unsigned int module_type) {
    ifstream file;
    stringstream code;
    string line;

    file.open(filepath);
    if (!file.is_open()) {
        cout << "error opening file";
        exit(0);
    }
    while (getline(file, line)) {
        code << line << '\n';
    }

    string shaderSource = code.str();
    const char* shaderSrc = shaderSource.c_str();
    code.str("");
    file.close();

    unsigned int shader = glCreateShader(module_type);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char elog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, elog);
        cout << "Error in compiling shader module" << elog << endl;
    }

    return shader;
}

unsigned int make_shader(const string& vertexFilepath, const string& fragFilepath) {
    vector<unsigned int> modules;

    modules.push_back(make_module(vertexFilepath, GL_VERTEX_SHADER));
    modules.push_back(make_module(fragFilepath, GL_FRAGMENT_SHADER));

    unsigned int shader = glCreateProgram();
    for (unsigned int a : modules) {
        glAttachShader(shader, a);
    }
    glLinkProgram(shader);

    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        char elog[1024];
        glGetProgramInfoLog(shader, 1024, NULL, elog);
        cout << "Error in linking program" << elog << endl;
    }

    for (unsigned int a : modules) {
        glDeleteShader(a);
    }

    return shader;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    distort = static_cast<float>(width)/height;
    width_sq = width_boxes / screen_width ;
    height_sq = height_boxes / screen_height;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    int cursor;
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if(screen == 0)
        {
            cursor = hovering();
            if(cursor == 1){
                glfwSetWindowShouldClose(window, true);
            }
            if(cursor == 2)
            {
                //high score
            }
            if(cursor == 3){
                //settings
            }
            if(cursor == 4){
                screen = 1;
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    bool equal = false;


    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Tetris", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    Grid screenGr;
    screenGr.initialize();

    Rect* background;
    background = new Rect();
    background -> set_rect_coords(0, 0, 0, 2, 2, false);

    std::vector<Rect*> title_elements;
    int element_colors = 6;
    auto add_rect = [&](int a, int b, int c, float w, float h, std::vector<Rect*>& elements){
    Rect* r = new Rect();
    r -> set_rect_coords(a, b, c, w * width_sq, h * height_sq, false);
    elements.push_back(r);
    };

    add_rect(12, 3, element_colors, 7, 1, title_elements);
    add_rect(8, 5, element_colors, 14, 2, title_elements);
    add_rect(10, 7, element_colors, 10, 2, title_elements);
    add_rect(11, 9, element_colors, 8, 2, title_elements);


    


    Material* bg = new Material("../img/tetris.png");
    Material* mask = new Material("../img/mask2.png");
    Material* def = new Material("../img/default.png");

    unsigned int game_shader = make_shader("shaders/vertex.txt", "shaders/fragment.txt");
    unsigned int anim_shader = make_shader("shaders/cus_vertex.txt", "shaders/cus_fragment.txt");
    unsigned int mask_shader = make_shader("shaders/vertex.txt","shaders/mask-fg.txt");
    unsigned int shader = anim_shader;

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "material"), 0);

    glUseProgram(mask_shader);
    glUniform1i(glGetUniformLocation(mask_shader, "mask"), 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<Square*> squares;
    int r = rand() % 7;
    int x = rand() % (right_wall - left_wall - 3) + left_wall;
    int y = rand() % 2 + height_boxes - 4;
    int c = (2 * rand() + x + y) % 6;
    switch(r){
        case 0: squares.push_back(new sq_block(x, y, c)); break;
        case 1: squares.push_back(new l_block(x, y, c)); break;
        case 2: squares.push_back(new rl_block(x, y, c)); break;
        case 3: squares.push_back(new long_block(x, y, c)); break;
        case 4: squares.push_back(new z_block(x, y, c)); break;
        case 5: squares.push_back(new rz_block(x, y, c)); break;
        case 6: squares.push_back(new new_block(x,y,c)); break;
        default:
            cout << "Error in generating random block";
            exit(-1);
    }


    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    Square* active = squares[0];
    float delta=0, cur, last = 0;

    int cursor;

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glfwPollEvents();

        //0 is title screen
        //1 is game screen
        //2 is settings screen

        if(screen == 0){
            shader = anim_shader;
            glUseProgram(anim_shader);

            
            bg -> use(0);
            background -> draw();


            //0 none
            //1 exit
            //2 high scores
            //3 settings
            //4 start
            cursor = hovering();

            glUseProgram(mask_shader);

            for(int i = 0; i<title_elements.size(); i++)
            {
                if(cursor != i+1){
                    glUseProgram(mask_shader);
                    mask -> use(0);
                    title_elements[i] -> draw();
                }
            }

        }
        else if(screen == 1){
            shader = game_shader;
            glUseProgram(shader);

        if(active -> get_collision()){
                x = rand() % (right_wall - left_wall - 3) + left_wall; 
                y = rand() % 2 + height_boxes - 5;                       
                r = rand() % 7;
                c = (rand() * rand() + x) % 6;
                switch(r){
                    case 0: squares.push_back(new sq_block(x, y, c)); break;   
                    case 1: squares.push_back(new l_block(x, y, c)); break;
                    case 2: squares.push_back(new rl_block(x, y, c)); break;
                    case 3: squares.push_back(new long_block(x, y, c)); break;
                    case 4: squares.push_back(new z_block(x, y, c)); break;
                    case 5: squares.push_back(new rz_block(x, y, c)); break;
                    case 6: squares.push_back(new new_block(x,y,c)); break;
                    default:
                        cout << "Error in generating random block";
                        exit(-1);
                }
            }

        for(int i=0; i<10; i++){
            if(floors[i] == height_boxes-4){
                cout << "Game Over" << endl;
                exit(0);
            }
        }


        //choose which object is active
        active = squares.back();




        for(Square* a : squares){
            a -> draw();
        }

        cur = glfwGetTime();
        delta = cur - last;
        if(delta > static_cast<float>(1)/frame_rate){
         //falling down
            //std::cout << "  testing    "; 
             for(Square* a : squares){
                a -> gdown();
            }
            last = cur;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && input_flag) {
            active -> gright();
            input_flag = false;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && input_flag) {
            active -> gleft();
            input_flag = false;
        }
        else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && input_flag){
            active -> instant_down();
            input_flag = false;
        }
        else if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && input_flag){
            active -> rotate();
            input_flag = false;
        }
        }
        else if(screen == 2){}
        else{
            cout << "Error in screen state" << endl;
            exit(-1);
        }

        screenGr.grid_draw();
        glfwSwapBuffers(window);
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
    
}