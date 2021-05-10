#include <map>
#include <string>
#include <sys/select.h>

#include "text.h"
#include "render.h"
#include "terminal.h"

#include <chrono>
using namespace std::chrono;

#define PADDING_LEFT 50
#define PADDING_BOTTOM 50
#define PADDING_TOP 50
#define PADDING_RIGHT 50
#define LINE_HEIGHT 20

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 720;

static std::map<GLchar, Character> characters;
static unsigned int VAO, VBO;
static Terminal term;
static PTY pty;
static unsigned bytes_to_ignore = 0;

/**
 * 
 * Okay -- first off, remember tha glBufferSubData replaces the data in the VBO.
 * When we render the buffer, get the difference between the current buffer and the last buffer we rendered
 * If there is no difference: Great! return and do nothing to the VBO
 * Otherwise, ONLY CONSTRUCT THE NEW VERTEX DATA FOR THE NEW DATA IN THE BUFFER, AND glBufferSubData AT THE OFFSET THAT YOU NEED
 * Hopefully, this will give the speedup i need. I don't think its possible to get around the "read one byte at a time problem"... but actually
 * i probably can i just don't know how. For now this seems fine.
 * 
 * It's late. Goodnight. see you future Ibrahim
 *
 */

void render_text(unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "text_color"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto const &c : text)
    {
        if (c == '\n')
        {
            x = PADDING_LEFT;
            y -= LINE_HEIGHT;
            continue;
        }
        Character ch = characters[c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == 0) // release
        return;

    switch (key)
    {
    case GLFW_KEY_ENTER:
    {
        const char *cmd = (term.get_cmd() + '\r').c_str();
        printf("cmd: %s\n", cmd);
        bytes_to_ignore = strlen(cmd) - 1;
        write(pty.master, cmd, strlen(cmd));
        term.new_line();
        break;
    }
    }
}

void char_cb(GLFWwindow *window, unsigned int c)
{
    term.add_char_cmd(c);
    term.set_char(c);
    term.move_cursor_right();
}

int main()
{
    // if (!pty_setup(&pty))
    //     return 1;
    // if (!spawn(&pty))
    //     return 1;

    // fd_set readable;
    // char buf[1];
    // char *cmd = "ls\n";

    // int i = 0;
    // while (1)
    // {
    //     FD_ZERO(&readable);
    //     FD_SET(pty.master, &readable);

    //     if (i == 100)
    //     {
    //         write(pty.master, cmd, 3);
    //     }

    //     if (FD_ISSET(pty.master, &readable))
    //     {
    //         if (read(pty.master, buf, 1) > 0)
    //         {
    //             // if (bytes_to_ignore > 0)
    //             //     {
    //             //         bytes_to_ignore--;
    //             //         continue;
    //             //     }
    //             //     term.set_char(buf[0]);
    //             //     term.move_cursor_right();
    //             printf("Byte received: %c\n", buf[0]);
    //             //     // for (int i = 0; i < 32; i++)
    //             //     // {
    //             //     //     term.set_char(buf[i]);
    //             //     //     term.move_cursor_right();
    //             //     // }
    //             // }
    //         }
    //     }
    //     i++;
    // }
    if (!pty_setup(&pty))
        return 1;
    if (!spawn(&pty))
        return 1;

    initGLFW();
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "fafterm", NULL, NULL);
    initGLEW(window);

    glfwSetCharCallback(window, char_cb);
    glfwSetKeyCallback(window, key_cb);

    unsigned int shader;
    init_render(SCR_WIDTH, SCR_HEIGHT, shader, VAO, VBO);

    FT_Library ft;
    FT_Face face;
    init_ft2(&ft, &face);
    load_characters(characters, face);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    fd_set readable;
    char buf[1];

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1568f, 0.1568f, 0.1568f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        FD_ZERO(&readable);
        FD_SET(pty.master, &readable);

        std::string line;

        if (FD_ISSET(pty.master, &readable))
        {
            // std::getline(pty.master, line);

            // read

            // printf("Line: %s\n", line.c_str());
            if (read(pty.master, buf, 1) > 0)
            {
                // printf("Bytes to ignore: %d\n", bytes_to_ignore);
                if (bytes_to_ignore > 0)
                {
                    bytes_to_ignore--;
                    continue;
                }
                term.set_char(buf[0]);
                term.move_cursor_right();
                // printf("Byte received: %c\n", buf[0]);
                // for (int i = 0; i < 32; i++)
                // {
                //     term.set_char(buf[i]);
                //     term.move_cursor_right();
                // }
            }
        }

        auto start = high_resolution_clock::now();
        render_text(shader, term.get_buf(), PADDING_LEFT, SCR_HEIGHT - LINE_HEIGHT - PADDING_TOP, 1, glm::vec3(0.94, 0.8588, 0.698));
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        // printf("Duration: %f\n", duration.count());
        std::cout << duration.count() << '\n';

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/**
 * 
 * 
 * 
 * 
 * 
 */