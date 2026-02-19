//
// Created by willb on 2/5/2026.
//

#include "stacktrace_debug_menu.h"

#include <iostream>
#include <nds.h>

void StackTraceDisplay()
{
    return;

    while (true)
    {
        scanKeys();
        if (keysDown() & KEY_B)
        {
            return;
        }
        consoleClear();

        void *array[10];
        size_t size;

        // // get void*'s for all entries on the stack
        // size = backtrace(array, 10);
        //
        // // print out all the frames to stderr
        // // printf("Error: signal %d:\n", sig);
        // backtrace_symbols_fd(array, size, STDERR_FILENO);
    }
}
