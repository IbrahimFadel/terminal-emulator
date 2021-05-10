#include "terminal.h"

void Terminal::set_char(char c)
{
    buf[y * cols + x] = c;
}

void Terminal::add_char_cmd(char c)
{
    cmd += c;
}

void Terminal::move_cursor_right()
{
    if (x == cols)
    {
        x = 0;
        if (y < rows)
            y++;
        else
            printf("Max rows reached\n");
    }
    else
        x++;
}

void Terminal::new_line()
{
    cmd.clear();
    while (x < cols - 1)
    {
        set_char(' ');
        move_cursor_right();
    }
    set_char('\n');
    x = 0;
    if (y < rows)
        y++;
    else
        printf("Max rows reached\n");
}

int pty_setup(PTY *pty)
{
    char *slave_name;

    pty->master = posix_openpt(O_RDWR | O_NOCTTY);
    if (pty->master == -1)
    {
        perror("posix_openpt");
        return 0;
    }

    if (grantpt(pty->master) == -1)
    {
        perror("grantpt");
        return 0;
    }
    if (unlockpt(pty->master) == -1)
    {
        perror("grantpt");
        return 0;
    }
    int flags = fcntl(pty->master, F_GETFL, 0);
    fcntl(pty->master, F_SETFL, flags | O_NONBLOCK);

    slave_name = ptsname(pty->master);
    if (slave_name == NULL)
    {
        perror("ptsname");
        return 0;
    }

    pty->slave = open(slave_name, O_RDWR | O_NOCTTY);
    if (pty->slave == -1)
    {
        perror("open(slave_name)");
        return 0;
    }

    return 1;
}

int spawn(PTY *pty)
{
    pid_t pid;
    const char *env[] = {"TERM=fafterm", NULL};

    pid = fork();
    if (pid == 0)
    {
        close(pty->master);

        setsid();
        if (ioctl(pty->slave, TIOCSCTTY, NULL) == -1)
        {
            perror("ioctl(TIOCSCTTY)");
            return 0;
        }

        dup2(pty->slave, 0);
        dup2(pty->slave, 1);
        dup2(pty->slave, 2);
        close(pty->slave);

        execle(SHELL, "-" SHELL, (char *)NULL, env);
        return 0;
    }
    else if (pid > 0)
    {
        close(pty->slave);
        return 1;
    }

    perror("fork");
    return 0;
}