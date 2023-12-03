#include "hidftp.h"

#include <dirent.h>
#include <sys/types.h>

int do_ls_r(char *path) {
  struct dirent *dp;
  static DIR *dir = NULL;

  if(path)
    dir = opendir(path);

  if(dir)
    dp = readdir(dir);

  if (dp) {
    dev.write(dp->d_name, strlen(dp->d_name));
    return 1;
  } else {
    Serial.printf("ls done\r\n");
    dev.write((char *)NULL, 0);
    closedir(dir);
    dir = NULL;
  }

  return 0;
}

int do_get_r(char *path) {
  char tmp[63];
  static FILE *f = NULL;

  if (path) {
    if (!(f = fopen(path, "r"))) {
      dev.write((char *)NULL, 0);
      return 0;
    };
  }

  size_t bytes = fread(tmp, 1, 63, f);

  if (bytes > 0) {
    dev.write(tmp, bytes);
    return 1;
  } else {
    Serial.printf("get done\r\n");    
    dev.write((char *)NULL, 0);
    fclose(f);
    f = NULL;
  }

  return 0;
}

int do_put_r(char *path, uint8_t const *buffer, uint16_t bufsize) {
  static FILE *f = NULL;

  if (path) {
    Serial.printf("Writing to %s\r\n", path);
    f = fopen(path, "w");
    return 1;
  }

  if (bufsize > 0 && f) {
    fwrite(buffer, bufsize, 1, f);
    return 1;
  } else {
    Serial.printf("put done\r\n");    
    fclose(f);
    f = NULL;
  }

  return 0;
}

void parse_cmd(const uint8_t *buf, uint16_t bufsiz, char **cmd, char **arg) {
  uint16_t *cmdlen = (uint16_t *)buf;
  *cmd = (char *)(buf + 2);
  uint16_t *arglen = (uint16_t *)(*cmd + *cmdlen);
  *arg = (char *)arglen + 2;

  Serial.printf("Command [%hu]: %s\r\n", *cmdlen, *cmd);
  Serial.printf("Arg [%hu]: %s\r\n", *arglen, *arg);
}
