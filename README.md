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

find-repos recursively searches the specified directory and prints the relative
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

To display usage information, use the `-h` or `--help` flag:

```sh
find-repos -h
```

## Integration

find-repos is designed to integrate with fuzzy finders. You can add a function
to your shell configuration file to quickly change directories to a repository.
For example, for zsh, add this function to `~/.zshrc`:

```sh
cdr() {
  local repo="$(find-repos | fzy)"
  [[ -n "$repo" ]] && cd ~/dev/"$repo"
}
```

This creates a `cdr` function that uses [fzy](https://github.com/jhawthorn/fzy)
to interactively select a repository and change to that directory. Similar
functions can be created for other shells and fuzzy finders such as
[fzf](https://github.com/junegunn/fzf).

## Development

find-repos is written in C99 for portability. A debug build with address and
undefined behaviour sanitisers is available for development:

```sh
make debug
```

Code formatting is handled by clang-format:

```sh
make format
```
