#!/bin/bash

SRC_DIR="src"
OBJ_DIR="build"
EXE="sx64-generic-emu"

CC="clang++"

CFLAGS="-Wall -Wextra -O3 -I$SRC_DIR"
LDFLAGS=""
INCLUDES=""
LIBS=""

NUM_JOBS=$(nproc)
FORCE_REBUILD=0

COLOR_RESET="\033[0m"
COLOR_WARN="\033[1;33m"
COLOR_ERROR="\033[1;31m"
COLOR_INFO="\033[1;32m"
COLOR_COMPILING="\033[1;34m"
COLOR_SKIPPING="\033[1;36m"
COLOR_LINKING="\033[1;32m"

print_help() {
    echo -e "${COLOR_INFO}Usage: $0 [OPTIONS]${COLOR_RESET}"
    echo ""
    echo -e "${COLOR_INFO}Options:${COLOR_RESET}"
    echo -e "  -B, --rebuild       ${COLOR_INFO}Force rebuild of all source files.${COLOR_RESET}"
    echo -e "  -c, --clean         ${COLOR_INFO}Clean the build directory.${COLOR_RESET}"
    echo -e "  -j, --jobs <N>      ${COLOR_INFO}Set the number of parallel jobs (default: number of CPU cores).${COLOR_RESET}"
    echo -e "  -o, --output <FILE> ${COLOR_INFO}Set the name of the output executable (default: sx64-generic-emu).${COLOR_RESET}"
    echo -e "  -h, --help          ${COLOR_INFO}Display this help message and exit.${COLOR_RESET}"
}

install_spdlog() {
    case $(uname -s) in
        Linux)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update
                sudo apt-get install -y libspdlog-dev
            elif command -v yum >/dev/null 2>&1; then
                sudo yum install -y spdlog-devel
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y spdlog-devel
            elif command -v pacman >/dev/null 2>&1; then
                sudo pacman -S --noconfirm spdlog
            else
                echo -e "${COLOR_ERROR}Unsupported package manager or operating system.${COLOR_RESET}"
                exit 1
            fi
            ;;
        Darwin)
            if command -v brew >/dev/null 2>&1; then
                brew install spdlog
            else
                echo -e "${COLOR_ERROR}Homebrew not found. Please install Homebrew or manually install spdlog.${COLOR_RESET}"
                exit 1
            fi
            ;;
        *)
            echo -e "${COLOR_ERROR}Unsupported operating system.${COLOR_RESET}"
            exit 1
            ;;
    esac
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -B|--rebuild)
            FORCE_REBUILD=1
            shift
            ;;
        -c|--clean)
            echo -e "${COLOR_WARN}Cleaning${COLOR_RESET} $OBJ_DIR"
            rm -rf "$OBJ_DIR"/*
            echo -e "${COLOR_INFO}Clean complete!${COLOR_RESET}"
            exit 0
            ;;
        -j|--jobs)
            if [[ -n "$2" && "$2" =~ ^[0-9]+$ ]]; then
                NUM_JOBS="$2"
                shift 2
            else
                echo -e "${COLOR_ERROR}Error: --jobs requires a numerical argument.${COLOR_RESET}"
                exit 1
            fi
            ;;
        -o|--output)
            if [[ -n "$2" ]]; then
                EXE="$2"
                shift 2
            else
                echo -e "${COLOR_ERROR}Error: --output requires a filename.${COLOR_RESET}"
                exit 1
            fi
            ;;
        -h|--help)
            print_help
            exit 0
            ;;
        *)
            echo -e "${COLOR_ERROR}Unknown option: $1${COLOR_RESET}"
            print_help
            exit 1
            ;;
    esac
done

if command -v pkg-config >/dev/null 2>&1; then
    if ! pkg-config --exists spdlog; then
        echo -e "${COLOR_WARN}spdlog not found. Installing...${COLOR_RESET}"
        install_spdlog
    fi
    echo -e "${COLOR_INFO}spdlog installed${COLOR_RESET}"
    INCLUDES=$(pkg-config --cflags spdlog)
    LIBS=$(pkg-config --libs spdlog)
else
    echo -e "${COLOR_ERROR}pkg-config is not installed. Please install pkg-config and spdlog manually.${COLOR_RESET}"
    exit 1
fi

mkdir -p "$OBJ_DIR"

compile_source() {
    src_file="$1"
    obj_file="${OBJ_DIR}/$(basename "${src_file%.*}.o")"

    if [[ $FORCE_REBUILD -eq 0 && -f "$obj_file" && "$obj_file" -nt "$src_file" ]]; then
        echo -e "${COLOR_SKIPPING}Skipping${COLOR_RESET} $src_file -> $obj_file (up to date)"
        return
    fi

    echo -e "${COLOR_COMPILING}Compiling${COLOR_RESET} $src_file -> $obj_file"
    if ! $CC $CFLAGS $INCLUDES -c "$src_file" -o "$obj_file" 2>&1; then
        echo -e "${COLOR_ERROR}Failed to compile $src_file${COLOR_RESET}"
        exit 1
    fi
}

PIDS=()

for src_file in $(find "$SRC_DIR" -type f -name "*.cpp"); do
    compile_source "$src_file" &
    PIDS+=($!)
    if (( ${#PIDS[@]} >= NUM_JOBS )); then
        wait -n
        PIDS=($(jobs -rp))
    fi
done

wait "${PIDS[@]}"

echo -e "${COLOR_LINKING}Linking${COLOR_RESET} object files into $EXE"
OBJ_FILES=$(find "$OBJ_DIR" -type f -name "*.o")
if [ -n "$OBJ_FILES" ]; then
    $CC $OBJ_FILES $LIBS -o "$EXE" 2>&1 | sed "s/^/  /"
    echo -e "${COLOR_INFO}Build complete!${COLOR_RESET} Executable: $EXE"
else
    echo -e "${COLOR_ERROR}No object files found for linking.${COLOR_RESET}"
fi      