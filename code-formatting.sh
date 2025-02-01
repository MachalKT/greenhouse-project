#!/bin/bash

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "clang-format is not installed. Please install it and try again."
    exit 1
fi

# Find and format all C/C++ files in the current directory and subdirectories, excluding 'build' folder
find . -type d -name "build" -prune -o -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 | xargs -0 clang-format -i --style=file

echo "Formatting completed."
