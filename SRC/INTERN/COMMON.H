#include <dos.h>

/* (Open)Watcom declares inp/outp in conio.h, not in dos.h. */
#if __WATCOMC__
#include <conio.h>
#endif

#if __TURBOC__
#define inline
#endif