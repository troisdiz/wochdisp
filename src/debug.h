#ifndef _DEBUG_H
#define _DEBUG_H

/* Define PRINTFUNC */
#ifdef DEBUG
#include <stdio.h>
#define PRINTFFUNC  printf("\n## %s (%s, l%d) ##\n", __FUNCTION__, __FILE__, __LINE__)
#define LOGDEBUG(TXT) printf("\n## %s (%s, l%d) ## : TXT\n", __FUNCTION__, __FILE__, __LINE__)
#define DEBUGFUNC(FUNC) printf("\n## %s (%s, l%d) ##\n", __FUNCTION__, __FILE__, __LINE__); FUNC
#endif
#ifndef DEBUG
#define PRINTFFUNC
#define LOGDEBUG(TXT)
#define DEBUGFUNC(FUNC)
#endif

#endif /* _DEBUG_H */
