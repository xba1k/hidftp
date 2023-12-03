#ifndef HIDFTP_H
#define HIDFTP_H

#include <Arduino.h>
#include "hidgeneric.h"

extern HIDgeneric dev;

void parse_cmd(const uint8_t *buf, uint16_t bufsiz, char **cmd, char **arg);
int do_ls_r(char *path);
int do_get_r(char *path);
int do_put_r(char *path, uint8_t const *buf, uint16_t buflen);

#endif
