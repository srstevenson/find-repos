#define _POSIX_C_SOURCE 200809L /* For realpath() etc. */

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int is_dir(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int is_git_repo(const char *path) {
  char buf[PATH_MAX];
  if (snprintf(buf, sizeof buf, "%s/.git", path) >= (int)sizeof buf) return 0;
  return is_dir(buf);
}

static void find_repos(const char *path, size_t base_len) {
  DIR *dir = opendir(path);
  if (!dir) return;

  for (struct dirent *ent; (ent = readdir(dir)) != NULL;) {
    // Skip "." and ".."
    if (ent->d_name[0] == '.' &&
        (ent->d_name[1] == '\0' ||
         (ent->d_name[1] == '.' && ent->d_name[2] == '\0')))
      continue;

    char child[PATH_MAX];
    if (snprintf(child, sizeof child, "%s/%s", path, ent->d_name) >=
        (int)sizeof child)
      continue;

    if (!is_dir(child)) continue;

    if (is_git_repo(child)) {
      // base_len is length of "~/dev" and +1 skips leading '/'
      puts(child + base_len + 1);
    } else {
      find_repos(child, base_len);
    }
  }

  closedir(dir);
}

int main(void) {
  const char *home = getenv("HOME");
  if (!home) {
    fputs("$HOME is not set\n", stderr);
    return EXIT_FAILURE;
  }

  char start[PATH_MAX];
  if (snprintf(start, sizeof start, "%s/dev", home) >= (int)sizeof start) {
    fputs("start path is too long\n", stderr);
    return EXIT_FAILURE;
  }

  char canonical[PATH_MAX];
  if (!realpath(start, canonical)) {
    fprintf(stderr, "realpath(%s): %s\n", start, strerror(errno));
    return EXIT_FAILURE;
  }

  find_repos(canonical, strlen(canonical));
  return EXIT_SUCCESS;
}
