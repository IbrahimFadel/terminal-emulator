#include "shader.h"

const char *read_file_content(const char *path)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(path, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = (char *)malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
        buffer[length] = '\0';
    }

    return buffer;
}

unsigned int load_shader(const char *vs_path, const char *fs_path)
{
    const char *vs_content = read_file_content(vs_path);
    const char *fs_content = read_file_content(fs_path);

    unsigned int vs_id = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs_id = glCreateShader(GL_FRAGMENT_SHADER);

    int result = 0;
    int info_log_len;
    printf("Compiling vertex shader: %s\n", vs_path);
    glShaderSource(vs_id, 1, &vs_content, NULL);
    glCompileShader(vs_id);

    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        char msg[info_log_len + 1];
        glGetShaderInfoLog(vs_id, info_log_len, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    printf("Compiling fragment shader: %s\n", fs_path);
    glShaderSource(fs_id, 1, &fs_content, NULL);
    glCompileShader(fs_id);

    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        char msg[info_log_len + 1];
        glGetShaderInfoLog(fs_id, info_log_len, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    printf("Linking shader program\n");
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs_id);
    glAttachShader(program, fs_id);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        char msg[info_log_len + 1];
        glGetProgramInfoLog(program, info_log_len, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    glDetachShader(program, vs_id);
    glDetachShader(program, fs_id);

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    return program;
}