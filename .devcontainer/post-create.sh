#!/usr/bin/env bash
# Builds and installs epiworldpy (with the test extras) into the container's
# virtual environment. Re-run `uv pip install .` after changing C++ sources.
set -euo pipefail

echo 'export LANG=C.UTF-8' >> "$HOME/.bashrc"
echo 'export LC_ALL=C.UTF-8' >> "$HOME/.bashrc"

uv pip install ".[test]"
