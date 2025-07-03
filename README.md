# find-repos

Find Git and Jujutsu repositories within a directory tree, and print their
relative paths. This is designed for use with a fuzzy finder to move easily
between repositories.

## Installation

A C compiler is required. Build and install using the provided Makefile:

```sh
make PREFIX=~/.local install
```

## Usage

The program recursively searches the specified directory and prints the relative
path of each Git repository (identified by the presence of a `.git` directory)
or Jujutsu repository (identified by the presence of a `.jj` directory).

By default, `find-repos` searches for repositories in `~/dev`:

```sh
find-repos
```

You can specify a different directory to search by passing a path as a single
argument:

```sh
find-repos ~/src
find-repos /path/to/projects
```
