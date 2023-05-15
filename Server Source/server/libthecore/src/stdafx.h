#pragma once

#ifdef _WIN32
#define INLINE inline
#else
#define INLINE __inline__
#endif

#ifndef _WIN32
#define false	0
#define true	(!false)
#endif

#define FALSE	false
#define TRUE	(!FALSE)

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <tchar.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>
#include <process.h>
#include <limits.h>
#include <math.h>
#include <locale.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>

#include "xdirent.h"
#include "xgetopt.h"

#define S_ISDIR(m)	(m & _S_IFDIR)

#define strlcat(dst, src, size) strcat_s(dst, size, src)
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#define strcasecmp(s1, s2) _stricmp(s1, s2)
#define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#define strtok_r(s, delim, ptrptr) strtok_s(s, delim, ptrptr)
#define localtime_r(timet, result) localtime_s(result, timet)

#define SIGUSR1     30
#define SIGUSR2     31

inline void usleep(unsigned long usec)
{
	Sleep(usec / 1000);
}

inline unsigned sleep(unsigned sec)
{
	Sleep(sec * 1000);
	return 0;
}

#define PATH_MAX _MAX_PATH
#else
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <sys/signal.h>
#include <sys/wait.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/event.h>
#endif

#include "typedef.h"
#include "heart.h"
#include "fdwatch.h"
#include "socket.h"
#include "kstbl.h"
#include "hangul.h"
#include "buffer.h"
#include "signal.h"
#include "log.h"
#include "main.h"
#include "utils.h"
#include "crypt.h"
#include "memcpy.h"