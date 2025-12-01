#include "client/client_ui.h"
#include "client/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void set_raw_mode(struct termios *old_tio) {
    struct termios new_tio;

    if (tcgetattr(STDIN_FILENO, old_tio) < 0) {
        perror("tcgetattr");
        exit(1);
    }

    new_tio = *old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    new_tio.c_cc[VMIN]  = 1;
    new_tio.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) < 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void restore_mode(const struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}
