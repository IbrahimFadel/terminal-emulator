#ifndef RENDER_H
#define RENDER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glBoilerplate.h"
#include "shader.h"

void init_render(int width, int height, unsigned int &shader, unsigned int &VAO, unsigned int &VBO);

#endif