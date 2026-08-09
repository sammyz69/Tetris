#include "config.h"
#include "square.h"
#include "globals.h"

bool input_flag = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    input_flag = true;
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

void set_background(enum state a) {
    switch (a) {
        case 0:
            glClearColor(0.25, 0.25, 0.5, 1.0);
            break;
        case 1:
            glClearColor(0, 0, 0, 1);
            break;
        default:
            cout << "Error in switching background";
            exit(-1);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    distort = static_cast<float>(width)/height;
    width_sq = width_boxes / screen_width ;
    height_sq = height_boxes / screen_height;
}

int main() {


    enum state Screen = Game;


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

    unsigned int shader = make_shader("shaders/vertex.txt", "shaders/fragment.txt");


    Grid screenGr;
    screenGr.initialize();

    std::vector<Square*> squares;
    int r = rand() % 6;
    int x = rand() % (width_boxes - right_wall) + left_wall;
    int y = rand() % 3 + height_boxes - 1;
    switch(r){
        case 0:
            squares.push_back(new sq_block(x, y, 0));
            break;
        case 1:
            squares.push_back(new l_block(x, y, 0));
            break;  
        case 2:
            squares.push_back(new rl_block(x, y, 0));
            break;
        case 3:
            squares.push_back(new long_block(x, y, 0));
            break;
        case 4:
            squares.push_back(new z_block(x, y, 0));
            break;
        case 5:
            squares.push_back(new rz_block(x, y, 0));
            break;
        default:
            cout << "Error in generating random block";
            exit(-1);
    }


    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    Square* active = squares[0];
    float delta=0, cur, last = 0;
    while (!glfwWindowShouldClose(window)) {
        set_background(Screen);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader);

        if(active -> get_collision()){
            x = rand() % (width_boxes - right_wall) + left_wall;
            y = rand() % 3 + height_boxes - 3;
            r = rand() % 6;
            switch(r){
                case 0:
                    squares.push_back(new sq_block(x, y, 0));
                    break;
                case 1:
                    squares.push_back(new l_block(x, y, 0));
                    break;  
                case 2:
                    squares.push_back(new rl_block(x, y, 0));
                    break;
                case 3:
                    squares.push_back(new long_block(x, y, 0));
                    break;
                case 4:
                    squares.push_back(new z_block(x, y, 0));
                    break;
                case 5:
                    squares.push_back(new rz_block(x, y, 0));
                    break;
                default:
                    cout << "Error in generating random block";
                    exit(-1);
            }
            
        }

        //choose which object is active
        active = squares.back();


        screenGr.grid_draw();

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
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && input_flag) {
            active -> gright();
            input_flag = false;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && input_flag) {
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


        glfwSwapBuffers(window);
    }



    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}