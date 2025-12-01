#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include <termios.h>

void set_raw_mode(struct termios *old_tio);
void restore_mode(const struct termios *old_tio);

#endif