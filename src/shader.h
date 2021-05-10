#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

const char *read_file_content(const char *path);
unsigned int load_shader(const char *vs_path, const char *fs_path);

#endif