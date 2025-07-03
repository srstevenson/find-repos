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

/* Helper function to build paths safely */
static int build_path(char *buf, size_t buf_size, const char *dir,
                      const char *name) {
  int result = snprintf(buf, buf_size, "%s/%s", dir, name);
  if (result >= (int)buf_size) {
    fprintf(stderr, "Warning: path too long, skipping %s/%s\n", dir, name);
    return 0;
  }
  return 1;
}

/* Check if directory name is "." or ".." */
static int is_dot_dir(const char *name) {
  return name[0] == '.' &&
         (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'));
}

/* Check if directory contains a repository of the given type */
static int is_repo_type(const char *path, const char *dirname) {
  char buf[PATH_MAX];
  if (!build_path(buf, sizeof buf, path, dirname)) {
    return 0;
  }
  return is_dir(buf);
}

static int is_git_repo(const char *path) { return is_repo_type(path, ".git"); }

static int is_jj_repo(const char *path) { return is_repo_type(path, ".jj"); }

static int is_repo(const char *path) {
  return is_git_repo(path) || is_jj_repo(path);
}

/* Recursively find repositories in directory tree */
static void find_repos(const char *dir_path, size_t base_len) {
  DIR *dir = opendir(dir_path);
  if (!dir) return;

  for (struct dirent *ent; (ent = readdir(dir)) != NULL;) {
    if (is_dot_dir(ent->d_name)) continue;

    char child[PATH_MAX];
    if (!build_path(child, sizeof child, dir_path, ent->d_name)) {
      continue;
    }

    if (!is_dir(child)) continue;

    if (is_repo(child)) {
      /* Print relative path (skip base directory and leading slash) */
      puts(child + base_len + 1);
    } else {
      /* Recursively search subdirectories */
      find_repos(child, base_len);
    }
  }

  if (closedir(dir) != 0) {
    fprintf(stderr, "Warning: failed to close directory %s: %s\n", dir_path,
            strerror(errno));
  }
}

static void usage(const char *program_name) {
  fprintf(stderr, "Usage: %s [directory]\n", program_name);
  fprintf(stderr, "Find Git and Jujutsu repos in the specified directory.\n");
  fprintf(stderr, "If no directory is specified, search in ~/dev\n");
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 2 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  char start[PATH_MAX];

  if (argc == 2) {
    /* Use provided directory */
    if (snprintf(start, sizeof start, "%s", argv[1]) >= (int)sizeof start) {
      fputs("provided path is too long\n", stderr);
      return EXIT_FAILURE;
    }
  } else {
    /* Use default ~/dev directory */
    const char *home = getenv("HOME");
    if (!home) {
      fputs("$HOME is not set\n", stderr);
      return EXIT_FAILURE;
    }

    if (snprintf(start, sizeof start, "%s/dev", home) >= (int)sizeof start) {
      fputs("start path is too long\n", stderr);
      return EXIT_FAILURE;
    }
  }

  char canonical[PATH_MAX];
  if (!realpath(start, canonical)) {
    fprintf(stderr, "realpath(%s): %s\n", start, strerror(errno));
    return EXIT_FAILURE;
  }

  find_repos(canonical, strlen(canonical));
  return EXIT_SUCCESS;
}
