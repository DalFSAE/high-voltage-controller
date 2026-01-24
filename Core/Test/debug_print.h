#pragma once

#ifndef DEBUG_PRINTF_ENABLE
#define DEBUG_PRINTF_ENABLE 1
#endif

#if DEBUG_PRINTF_ENABLE
    #include <stdio.h>
    #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...) ((void)0)
#endif
