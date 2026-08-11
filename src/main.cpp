#include "config.h"
#include "square.h"
#include "globals.h"
#include "material.h"
#include <algorithm>
#include <vector>
#include <string>

Material* digit_tex[10] = {nullptr};
Material* score_label = nullptr;
Material* congrats_tex = nullptr;

std::vector<Square*> squares;
std::vector<Rect*> static_blocks;
std::vector<Rect*> game_elements;
Square* active = nullptr;

void draw_number(int value, int grid_x, int grid_y, float scale = 1.0f) {
    if (value < 0) value = 0;
    std::string s = std::to_string(value);

    for (size_t i = 0; i < s.size(); i++) {
        int d = s[i] - '0';
        if (d < 0 || d > 9 || !digit_tex[d]) continue;

        Rect r;
        r.set_rect_coords(
            grid_x + static_cast<int>(i * scale),
            grid_y,
            7,
            width_sq * scale,
            height_sq * scale,
            false);
        digit_tex[d]->use(0);
        r.draw();
    }
}

bool input_flag = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    input_flag = true;
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    cursor_xpos = (xpos / screen_width) * width_boxes;
    cursor_ypos = ((screen_height - ypos) / screen_height) * height_boxes;
}

int hovering() {
    if ((cursor_xpos >= 12 && cursor_xpos <= 18) && (cursor_ypos == 3)) return 1;
    if ((cursor_xpos >= 8 && cursor_xpos <= 21) && (cursor_ypos >= 5 && cursor_ypos <= 6)) return 2;
    if ((cursor_xpos >= 10 && cursor_xpos <= 19) && (cursor_ypos >= 7 && cursor_ypos <= 8)) return 3;
    if ((cursor_xpos >= 11 && cursor_xpos <= 18) && (cursor_ypos >= 9 && cursor_ypos <= 10)) return 4;
    return 0;
}

void clean_game() {
    for (auto* s : squares) delete s;
    squares.clear();
    for (auto* r : static_blocks) delete r;
    static_blocks.clear();
    active = nullptr;
}

Square* spawn_block() {
    int play_width = right_wall - left_wall;
    int spawn_range = std::max(1, play_width - 2);

    for (int attempt = 0; attempt < 30; ++attempt) {
        int x = rand() % spawn_range + left_wall;
        if (x + 2 >= right_wall) x = std::max(left_wall, right_wall - 3);
        int y = height_boxes - 4;
        int shape = rand() % 7;
        int col = rand() % 6;

        Square* p = nullptr;
        switch (shape) {
            case 0: p = new sq_block(x, y, col); break;
            case 1: p = new l_block(x, y, col); break;
            case 2: p = new rl_block(x, y, col); break;
            case 3: p = new long_block(x, y, col); break;
            case 4: p = new z_block(x, y, col); break;
            case 5: p = new rz_block(x, y, col); break;
            default: p = new new_block(x, y, col); break;
        }

        if (p->is_spawn_position_free())
            return p;
        delete p;
    }
    return nullptr;
}

void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && screen == 0) {
        int h = hovering();
        if (h == 1) {
            glfwSetWindowShouldClose(w, true);
        } else if (h == 2) {
            std::cout << "=== HIGH SCORES ===\n";
            for (int i = 0; i < 3; i++)
                std::cout << (i + 1) << ". " << high_scores[i] << "\n";
            std::cout << "==================\n";
        } else if (h == 4) {
            clean_game();
            reset_playfield();
            user_score = 0;
            Square* first = spawn_block();
            if (first) {
                squares.push_back(first);
                active = first;
                screen = 1;
            }
        }
    }
}

unsigned int make_module(const string& filepath, unsigned int module_type) {
    ifstream file;
    stringstream code;
    string line;

    file.open(filepath);
    if (!file.is_open()) {
        cout << "error opening file: " << filepath << endl;
        exit(0);
    }
    while (getline(file, line)) code << line << '\n';

    string shaderSource = code.str();
    const char* shaderSrc = shaderSource.c_str();
    file.close();

    unsigned int shader = glCreateShader(module_type);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char elog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, elog);
        cout << "Error in compiling shader module (" << filepath << "): " << elog << endl;
        exit(-1);
    }
    return shader;
}

unsigned int make_shader(const string& vertexFilepath, const string& fragFilepath) {
    vector<unsigned int> modules;
    modules.push_back(make_module(vertexFilepath, GL_VERTEX_SHADER));
    modules.push_back(make_module(fragFilepath, GL_FRAGMENT_SHADER));

    unsigned int shader = glCreateProgram();
    for (unsigned int a : modules) glAttachShader(shader, a);
    glLinkProgram(shader);

    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        char elog[1024];
        glGetProgramInfoLog(shader, 1024, NULL, elog);
        cout << "Error in linking program: " << elog << endl;
        exit(-1);
    }
    for (unsigned int a : modules) glDeleteShader(a);
    return shader;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    distort = static_cast<float>(width) / height;
    width_sq = 2.0f / width_boxes;
    height_sq = 2.0f / height_boxes;
}

void rebuild_static_blocks(std::vector<Rect*>& static_blocks) {
    for (Rect* r : static_blocks) delete r;
    static_blocks.clear();

    int cols = right_wall - left_wall;
    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < height_boxes; r++) {
            if (board[c][r] >= 0) {
                Rect* cell = new Rect();
                cell->set_rect_coords(left_wall + c, r, board[c][r], width_sq, height_sq, false);
                static_blocks.push_back(cell);
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    reset_playfield();

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

    Rect* background = new Rect();
    background->set_rect_coords(0, 0, 0, 2, 2, false);

    std::vector<Rect*> title_elements;
    int element_colors = 6;
    auto add_rect = [&](int a, int b, int c, float w, float h, std::vector<Rect*>& elements) {
        Rect* r = new Rect();
        r->set_rect_coords(a, b, c, w * width_sq, h * height_sq, false);
        elements.push_back(r);
    };

    add_rect(12, 3, element_colors, 7, 1, title_elements);
    add_rect(8, 5, element_colors, 14, 2, title_elements);
    add_rect(10, 7, element_colors, 10, 2, title_elements);
    add_rect(11, 9, element_colors, 8, 2, title_elements);

    Material* bg = new Material("../img/tetris.png");
    Material* mask = new Material("../img/mask2.png");

    unsigned int game_shader = make_shader("shaders/vertex.txt", "shaders/fragment.txt");
    unsigned int anim_shader = make_shader("shaders/cus_vertex.txt", "shaders/cus_fragment.txt");
    unsigned int mask_shader = make_shader("shaders/vertex.txt", "shaders/mask-fg.txt");

    glUseProgram(anim_shader);
    glUniform1i(glGetUniformLocation(anim_shader, "material"), 0);
    glUseProgram(mask_shader);
    glUniform1i(glGetUniformLocation(mask_shader, "mask"), 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 10; i++) {
        std::string path = "../img/" + std::to_string(i) + ".png";
        digit_tex[i] = new Material(path.c_str());
    }
    score_label = new Material("../img/score.png");
    congrats_tex = new Material("../img/congrats.png");
    load_high_scores();

    game_elements.clear();
    add_rect(0, 0, 7, left_wall, height_boxes, game_elements);
    add_rect(right_wall, 0, 7, width_boxes - right_wall, height_boxes, game_elements);

    float delta = 0, cur = 0, last = 0;
    int cursor = 0;

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (screen == 0) {
            glUseProgram(anim_shader);
            bg->use(0);
            background->draw();

            cursor = hovering();
            glUseProgram(mask_shader);
            for (size_t i = 0; i < title_elements.size(); i++) {
                if (cursor != static_cast<int>(i) + 1) {
                    mask->use(0);
                    title_elements[i]->draw();
                }
            }
        }
        else if (screen == 1) {
            // safety
            if (squares.empty()) {
                Square* p = spawn_block();
                if (p) {
                    squares.push_back(p);
                    active = p;
                } else {
                    clean_game();
                    if (try_insert_high_score(user_score)) screen = 3;
                    else screen = 0;
                }
            }
            if (!squares.empty())
                active = squares.back();

            // drawing
            glUseProgram(game_shader);
            for (Rect* a : game_elements) a->draw();
            for (Rect* a : static_blocks) a->draw();
            for (Square* a : squares) a->draw();

            glUseProgram(anim_shader);
            if (score_label) {
                Rect label;
                label.set_rect_coords(1, height_boxes - 2, 7, width_sq * 4, height_sq, false);
                score_label->use(0);
                label.draw();
            }
            draw_number(user_score, 6, height_boxes - 2, 1.0f);

            // game-over by height
            bool game_over = false;
            int play_width = right_wall - left_wall;
            for (int i = 0; i < play_width; i++) {
                if (floors[i] >= height_boxes - 4) {
                    game_over = true;
                    break;
                }
            }

            if (game_over) {
                clean_game();
                if (try_insert_high_score(user_score)) screen = 3;
                else screen = 0;
            }
            else if (active) {
                // lock + spawn
                if (active->get_collision()) {
                    active->lock_piece();
                    rebuild_static_blocks(static_blocks);

                    delete active;
                    squares.pop_back();
                    active = nullptr;

                    Square* next = spawn_block();
                    if (!next) {
                        clean_game();
                        if (try_insert_high_score(user_score)) screen = 3;
                        else screen = 0;
                    } else {
                        squares.push_back(next);
                        active = next;
                    }
                }

                // gravity
                cur = glfwGetTime();
                delta = cur - last;
                if (delta > 1.0f / frame_rate) {
                    if (active) active->gdown();
                    last = cur;
                }

                // input
                if (active) {
                    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && input_flag) {
                        active->gright();
                        input_flag = false;
                    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && input_flag) {
                        active->gleft();
                        input_flag = false;
                    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && input_flag) {
                        active->instant_down();
                        input_flag = false;
                    } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && input_flag) {
                        active->rotate();
                        input_flag = false;
                    }
                }
            }
        }
        else if (screen == 3) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(anim_shader);
            if (congrats_tex) {
                Rect full;
                full.set_rect_coords(0, 0, 7, 2.0f, 2.0f, false);
                congrats_tex->use(0);
                full.draw();
            }

            if (input_flag) {
                input_flag = false;
                screen = 0;
                user_score = 0;
            }
        }
        else {
            cout << "Error in screen state" << endl;
            exit(-1);
        }

        glfwSwapBuffers(window);
    }

    // cleanup
    clean_game();
    for (auto* r : title_elements) delete r;
    for (auto* r : game_elements) delete r;
    delete background;
    for (int i = 0; i < 10; i++) delete digit_tex[i];
    delete score_label;
    delete congrats_tex;
    delete bg;
    delete mask;

    glDeleteProgram(game_shader);
    glDeleteProgram(anim_shader);
    glDeleteProgram(mask_shader);
    glfwTerminate();
    return 0;
}