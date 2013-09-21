#ifndef _USERDEF_H_
#define _USERDEF_H_


//typedef     unsigned char   BYTE;      // 1 byte
//typedef     unsigned short  WORD;      // 2 bytes
//typedef     unsigned long   DWORD;     // 4 bytes
typedef     int             INT;       // 4 bytes
//typedef     void *          HANDLE;

#ifdef NULL
#undef NULL
#endif

/*#ifdef IN
#undef IN
#endif*/
/*
#ifdef OUT
#undef OUT
#endif*/

#define NULL    0
//#define IN  __in
//#define OUT __out

#endif // _USERDEF_H_

