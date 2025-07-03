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

static int is_dot_dir(const char *name) {
  return name[0] == '.' &&
         (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'));
}

static int is_repo_type(const char *path, const char *dirname) {
  char buf[PATH_MAX];
  if (snprintf(buf, sizeof buf, "%s/%s", path, dirname) >= (int)sizeof buf) {
    fprintf(stderr, "Warning: path too long, skipping %s/%s\n", path, dirname);
    return 0;
  }
  return is_dir(buf);
}

static int is_git_repo(const char *path) { return is_repo_type(path, ".git"); }

static int is_jj_repo(const char *path) { return is_repo_type(path, ".jj"); }

static int is_repo(const char *path) {
  return is_git_repo(path) || is_jj_repo(path);
}

static void find_repos(const char *dir_path, size_t base_len) {
  DIR *dir = opendir(dir_path);
  if (!dir) return;

  for (struct dirent *ent; (ent = readdir(dir)) != NULL;) {
    if (is_dot_dir(ent->d_name)) continue;

    char child[PATH_MAX];
    if (snprintf(child, sizeof child, "%s/%s", dir_path, ent->d_name) >=
        (int)sizeof child) {
      fprintf(stderr, "Warning: path too long, skipping %s/%s\n", dir_path,
              ent->d_name);
      continue;
    }

    if (!is_dir(child)) continue;

    if (is_repo(child)) {
      // base_len is length of "~/dev" and +1 skips leading '/'
      puts(child + base_len + 1);
    } else {
      find_repos(child, base_len);
    }
  }

  if (closedir(dir) != 0) {
    fprintf(stderr, "Warning: failed to close directory %s: %s\n", dir_path,
            strerror(errno));
  }
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
