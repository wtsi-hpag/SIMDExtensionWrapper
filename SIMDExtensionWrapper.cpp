/*
Copyright (c) 2019 Ed Harry, Wellcome Sanger Institute

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cpuid.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define WINVER 0x0601 // Target Windows 7 as a Minimum Platform
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <process.h>
#include <io.h>
#define close _close
#define dup _dup
#define dup2 _dup2
#define execvp _execvp
#else
#include <unistd.h>
#endif

typedef uint32_t u32;
typedef uint8_t u08;
typedef int32_t s32;
#define global_variable static
#define ArgCount argc
#define ArgBuffer argv
#define MainArgs s32 main(s32 ArgCount, const char *ArgBuffer[])
#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))
#define ForLoop(n) for (u32 index = 0; index < (n); ++index)

#pragma clang diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wextra-semi-stmt"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wconditional-uninitialized"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#pragma clang diagnostic pop

global_variable
u08
Status_Marco_Expression_Sponge = 0;

global_variable
char
Message_Buffer[1024];

#ifdef ProgramName

#define ProgramNameString String(ProgramName)

#define String_(v) #v
#define String(v) String_(v)

#define PrintError(message, ...) \
{ \
stbsp_snprintf(Message_Buffer, 512, message, ##__VA_ARGS__); \
stbsp_snprintf(Message_Buffer + 512, 512, "[%s error] :: %s\n", ProgramNameString, Message_Buffer); \
fprintf(stderr, "%s", Message_Buffer + 512); \
} \
Status_Marco_Expression_Sponge = 0

#define PrintStatus(message, ...) \
{ \
stbsp_snprintf(Message_Buffer, 512, message, ##__VA_ARGS__); \
stbsp_snprintf(Message_Buffer + 512, 512, "[%s status] :: %s\n", ProgramNameString, Message_Buffer); \
fprintf(stdout, "%s", Message_Buffer + 512); \
} \
Status_Marco_Expression_Sponge = 0

#else

#define PrintError(message, ...) \
{ \
stbsp_snprintf(Message_Buffer, 512, message, ##__VA_ARGS__); \
fprintf(stderr, "%s\n", Message_Buffer); \
} \
Status_Marco_Expression_Sponge = 0

#define PrintStatus(message, ...) \
{ \
stbsp_snprintf(Message_Buffer, 512, message, ##__VA_ARGS__); \
fprintf(stdout, "%s\n", Message_Buffer); \
} \
Status_Marco_Expression_Sponge = 0

#endif

struct
entry
{
    u32 *extAvailable;
    char *name;
    char *extName;
};

MainArgs
{
    (void)ArgCount;

#ifdef _WIN32
    u08 exeStrip = 0;
    {
        char buff[1024];
        char *ptr = (char *)ArgBuffer[0];
        u32 len = 1;
        while (*++ptr) ++len;
        if (len > 4 && *(ptr - 4) == '.' && *(ptr - 3) == 'e' && *(ptr - 2) == 'x' && *(ptr - 1) == 'e')
        {
            exeStrip = 1;
            ForLoop((len - 4))
            {
                buff[index] = *(ArgBuffer[0] + index);
            }
            buff[len - 4] = 0;
            ArgBuffer[0] = (const char *)buff;
        }
    }
#endif

    u32 hasSSE41 = 0;
    u32 hasSSE42 = 0;
    u32 hasAVX = 0;
    u32 hasAVX2 = 0;

    u32 hasNoExt = 1;

    entry entries[] = {
        {
            &hasAVX2,
            (char *)"AVX2",
            (char *)"avx2"
        },
        {
            &hasAVX,
            (char *)"AVX",
            (char *)"avx"
        },
        {
            &hasSSE42,
            (char *)"SSE4.2",
            (char *)"sse42"
        },
        {
            &hasSSE41,
            (char *)"SSE4.1",
            (char *)"sse41"
        },
        {
            &hasNoExt,
            (char *)"no",
            (char *)"noext"
        }
    };

    {
        u32 a, b, c, d;
        if (__get_cpuid_count(1, 0, &a, &b, &c, &d))
        {
            hasSSE41 = c & (1 << 19);
            hasSSE42 = c & (1 << 20);
            hasAVX = c & (1 << 28);
        }
        if (__get_cpuid_count(7, 0, &a, &b, &c, &d))
        {
            hasAVX2 = b & (1 << 5);
        }
    }

    char buffer[1024];
    char *prosessName = 0;
    char *ext = 0;
    
    s32 dupout = dup(1);
    s32 duperr = dup(2);
    close(1);
    close(2);
    ForLoop(ArrayCount(entries))
    {
        entry *ent = entries + index;
        if (*(ent->extAvailable))
        {
            ext = ent->name;
#ifdef _WIN32
            if (exeStrip) stbsp_snprintf(buffer, sizeof(buffer), "%s.%s.exe", ArgBuffer[0], ent->extName);
            else stbsp_snprintf(buffer, sizeof(buffer), "%s.%s", ArgBuffer[0], ent->extName);
#else
            stbsp_snprintf(buffer, sizeof(buffer), "%s.%s", ArgBuffer[0], ent->extName);
#endif       
            if (!system(buffer))
            {
                prosessName = (char *)buffer;
                break;
            }
        }
    }
    dup2(dupout, 1);
    dup2(duperr, 2);
    close(dupout);
    close(duperr);

    if (prosessName)
    {
        PrintStatus("Running with %s CPU extensions", ext);
        ArgBuffer[0] = (const char *)prosessName;
        
        execvp((const char *)prosessName, (char *const *)ArgBuffer);
        PrintError("Error executing process \'%s\'", prosessName);
        exit(errno);
    }
    
    PrintError("CPU architecture not supported");
    exit(EXIT_FAILURE);
}
