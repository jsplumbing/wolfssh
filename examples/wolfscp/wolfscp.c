/* wolfscp.c
 *
 * Copyright (C) 2014-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSH.
 *
 * wolfSSH is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolfSSH.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifdef WOLFSSL_USER_SETTINGS
    #include <wolfssl/wolfcrypt/settings.h>
#else
    #include <wolfssl/options.h>
#endif

#include <wolfssh/ssh.h>
#include <wolfssh/log.h>
#include <wolfssh/test.h>
#include "wolfscp.h"


static void ShowUsage(void)
{
    printf("wolfscp %s\n", LIBWOLFSSH_VERSION_STRING);
    printf(" -?            display this help and exit\n");
    printf(" -p <num>      port to connect on, default %d\n", wolfSshPort);
    printf(" -s <filepath> file source, \"[[user@]addr:]file\"\n");
    printf(" -d <filepath> file destination, \"[[user@]addr:]file\"\n");
}


/* s is a non-const string containing the file source or destination in the
 * form "[[user@]addr:]file". The string s is modified with nulls terminating
 * the components returned. user loses the @. addr loses the :. The file
 * string is broken into a path and file name.
 */
typedef struct FilePath {
    const char* user;
    const char* addr;
    const char* file;
} FilePath;

static int ParseFilePath(char* s, FilePath* fp)
{
    char* pivot;

    if (s == NULL || fp == NULL)
        return -1;

    pivot = strchr(s, '@');
    if (pivot != NULL) {
        fp->user = s;
        *pivot = '\0';
        s = pivot + 1;
    }
    else {
        fp->user = NULL;
    }

    pivot = strchr(s, ':');
    if (pivot != NULL) {
        fp->addr = s;
        *pivot = '\0';
        s = pivot + 1;
    }
    else {
        fp->addr = NULL;
    }

    fp->file = s;

    return 0;
}


THREAD_RETURN WOLFSSH_THREAD wolfSSH_scp(void* args)
{
    word16      port = wolfSshPort;
    char        ch;
    int         argc = ((func_args*)args)->argc;
    char**      argv = ((func_args*)args)->argv;
    int         fromFlag = 0;
    int         toFlag = 0;
    char*       toPath = NULL;
    char*       sourceRaw = NULL;
    char*       destRaw = NULL;
    FilePath    sourcePath;
    FilePath    destPath;
    int*        returnCode = &((func_args*)args)->return_code;

    *returnCode = 0;

    while ((ch = mygetopt(argc, argv, "?p:ft:s:d:")) != -1) {
        switch (ch) {
            case '?': /* usage help */
                ShowUsage();
                exit(EXIT_SUCCESS);

            case 'p': /* port number */
                port = (word16)atoi(myoptarg);
                #if !defined(NO_MAIN_DRIVER) || defined(USE_WINDOWS_API)
                    if (port == 0)
                        err_sys("port number cannot be 0");
                #endif
                    break;

            case 'f': /* from source */
                    fromFlag = 1;
                    break;

            case 't': /* to sink */
                    toFlag = 1;
                    toPath = myoptarg;
                    break;

            case 's':
                    sourceRaw = myoptarg;
                    break;

            case 'd':
                    destRaw = myoptarg;
                    break;

            default:
                ShowUsage();
                exit(MY_EX_USAGE);
        }
    }
    myoptind = 0;      /* reset for test cases */

    printf("wolfSCP demonstration\n Using port %u.\n", port);

    if (fromFlag && toFlag) {
        printf(" Error: cannot source and sink at the same time.\n");
        *returnCode = 1;
        return NULL;
    }

    if (fromFlag)
        printf(" Sourcing a file.\n");

    if (toFlag)
        printf(" Sinking a file to path %s.\n", toPath);

    if (sourceRaw == NULL) {
        printf(" Error: source file path required.\n");
        *returnCode = 1;
        return NULL;
    }
    else {
        printf(" Source file path: %s\n", sourceRaw);
        ParseFilePath(sourceRaw, &sourcePath);
        printf("  user = %s\n  addr = %s\n  file = %s\n",
               sourcePath.user, sourcePath.addr, sourcePath.file);
    }

    if (destRaw == NULL) {
        printf(" Error: destination file path required.\n");
        *returnCode = 1;
        return NULL;
    }
    else {
        printf(" Destination file path: %s\n", destRaw);
        ParseFilePath(destRaw, &destPath);
        printf("  user = %s\n  addr = %s\n  file = %s\n",
               destPath.user, destPath.addr, destPath.file);
    }

    return NULL;
}

#ifndef NO_MAIN_DRIVER

    int main(int argc, char** argv)
    {
        func_args args;

        args.argc = argc;
        args.argv = argv;
        args.return_code = 0;

        WSTARTTCP();

        #ifdef DEBUG_WOLFSSH
            wolfSSH_Debugging_ON();
        #endif

        wolfSSH_Init();

        wolfSSH_scp(&args);

        wolfSSH_Cleanup();

        return args.return_code;

        start_thread(NULL, NULL, NULL);
        join_thread(0);
        detach_thread(0);
    }


    int myoptind = 0;
    char* myoptarg = NULL;

#endif /* NO_MAIN_DRIVER */
