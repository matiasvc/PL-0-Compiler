#include "util.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global.h"

void error(const char* fmt, ...) {
  va_list ap;

  (void) fprintf(stderr, "pl0c: error: %lu ", g.line);

  va_start(ap, fmt);
  (void) vfprintf(stderr, fmt, ap);
  va_end(ap);

  (void) fputc('\n', stderr);

  exit(1);
}

void readin(char* file) {
  int fd;
  struct stat st;

  if (strrchr(file, '.') == NULL) {
    error("file must end in '.pl0'");
  }

  if (strcmp(strrchr(file, '.'), ".pl0") != 0) {
    error("file must end in '.pl0'");
  }

  if ((fd = open(file, O_RDONLY)) == -1) {
    error("couldn't open %s", file);
  }

  if (fstat(fd, &st) == -1) {
    error("coudn't get file size");
  }

  // Allocate space for entire file pluss null-character
  if ((g.raw = (char*)malloc(st.st_size + 1)) == NULL) {
    error("malloc failed");
  }

  if (read(fd, g.raw, st.st_size) != st.st_size) {
    error("couldn't read %s", file);
  }

  // Terminate the string
  g.raw[st.st_size] = '\0';

  (void) close(fd);
}

void aout(const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  (void) vfprintf(stdout, fmt, ap);
  va_end(ap);
}
