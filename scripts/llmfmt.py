"""Concatenate all .cpp files in a directory, separating them with '---' and adding a comment with the filename."""

import os
import sys


def concat_cpp_files(directory):
    cpp_files = [f for f in os.listdir(directory) if f.endswith(".cpp")]
    cpp_files.sort()

    for i, filename in enumerate(cpp_files):
        filepath = os.path.join(directory, filename)
        print(f"// {filename}")
        with open(filepath, "r", encoding="utf-8") as f:
            sys.stdout.write(f.read())
        if i < len(cpp_files) - 1:
            print("\n---\n")


if __name__ == "__main__":
    directory = "." if len(sys.argv) < 2 else sys.argv[1]
    concat_cpp_files(directory)
