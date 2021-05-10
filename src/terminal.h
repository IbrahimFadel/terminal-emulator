#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>

#define SHELL "/bin/sh"

typedef struct PTY
{
    int master, slave;
} PTY;

class Terminal
{
private:
    unsigned rows, cols;
    unsigned x, y;
    char *buf;
    std::string cmd;

public:
    Terminal() : rows(50), cols(80), x(0), y(0)
    {
        buf = (char *)calloc(rows * cols, sizeof(char));
    };
    void set_char(char c);
    void add_char_cmd(char c);
    void move_cursor_right();
    void new_line();

    char *get_buf() { return buf; }
    std::string get_cmd() { return cmd; }
};

int pty_setup(PTY *pty);
int spawn(PTY *pty);

#endif