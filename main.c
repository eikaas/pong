#include <stdio.h>
#include <stdlib.h>
#include "pong.c"

int main(int argc, const char *argv[]) {

    if (init() == FALSE) {
        printf("Initialization failed\n");
        return EXIT_FAILURE;
    }

    if (loadFiles() == FALSE) {
        printf("failed loading files\n");
        return EXIT_FAILURE;
    }

    gameLoop();

    cleanup();

    return EXIT_SUCCESS;
}

