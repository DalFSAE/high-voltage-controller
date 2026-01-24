#pragma once

#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF 1
#endif

#if DEBUG_PRINTF
    #include <stdio.h>
    #define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DBG_PRINTF(...) ((void)0)
#endif
