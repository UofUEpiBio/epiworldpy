# Agent guide for epiworldpy

epiworldpy is a pybind11 wrapper around the header-only C++ library
[epiworld](https://github.com/UofUEpiBio/epiworld). The bindings live in
`epiworldpy/*.cpp`, and a copy of epiworld's headers is vendored in
`epiworldpy/include/epiworld/`.

## Always work inside the devcontainer

Run **every** build, test, formatting, and docs command inside the
devcontainer (`.devcontainer/`), never on the host. The container pins the
toolchain (Ubuntu 24.04, GCC 13, CMake, clang-format, Python 3.12 via uv), so
results match between agents, contributors, and CI (Linux).

From the repository root, with the [devcontainer CLI](https://github.com/devcontainers/cli):

```bash
# Start (or reuse) the container. The first run builds the image and installs
# epiworldpy[test] into /home/vscode/.venv (see .devcontainer/post-create.sh).
devcontainer up --workspace-folder .

# Run a command inside it.
devcontainer exec --workspace-folder . <command>
```

If Docker is not available but Podman is, add `--docker-path podman` to both
commands.

The container's virtual environment is at `/home/vscode/.venv`, outside the
workspace, and is already on `PATH`. Do not create or use a `.venv/` in the
repository from inside the container.

## Common tasks (run via `devcontainer exec --workspace-folder . …`)

| Task | Command |
| --- | --- |
| Rebuild and install after changing C++ or Python sources | `uv pip install .` |
| Run the tests | `pytest` |
| Format the binding sources | `make format` |
| Update the vendored epiworld headers | `make update` (expects an epiworld checkout at `../epiworld`) |

Notes:

- Run `pytest`, not `python -m pytest`. `python -m` puts the repository root on
  `sys.path`, so `import epiworldpy` picks up the source directory, which has
  no compiled `_core`, instead of the installed package. For the same reason,
  run ad hoc `python -c "import epiworldpy"` checks from outside the
  repository root (for example, from `tests/`).
- The extension is compiled at install time. After changing any `.cpp`/`.hpp`,
  run `uv pip install .` again before testing.
- The container's clang-format is newer than the one used to format the
  existing sources, so `make format` also reflows untouched code (issue #4).
  Commit only the formatting of code you changed.

## Updating epiworld

1. Copy the headers from the target epiworld release or commit into
   `epiworldpy/include/epiworld/`, using `make update` or `rsync --delete`.
   Keep them identical to upstream: fix upstream bugs upstream (and work
   around them in the bindings) instead of patching the vendored headers.
2. Bump `version` in `pyproject.toml` and `CITATION.cff` to match the epiworld
   version (for example, `0.17.0-0`).
3. Rebuild, run the tests, and check `epiworldpy.__epiworld_version__`.
