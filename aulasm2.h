/*
*
*                                 Apache License
*                           Version 2.0, January 2004
*                        http://www.apache.org/licenses/
*/

#ifndef __AULASM2_H__
#define __AULASM2_H__

#include "asmkernel.h"

typedef struct _CMDLINE
{
  unsigned int  spos;
  unsigned int  epos;
  unsigned int  length;
  unsigned char buf[8192];
} CMDLINE;

typedef struct _CMDARGS
{
  unsigned char  fname[512];
  unsigned int   file;
  unsigned short disk;
  unsigned int   aun;
  unsigned short block;
  unsigned char  level;
  unsigned int   count;
  unsigned int   offset;
} CMDARGS;

#define  MIN(a,b) ((a) > (b) ? (b) : (a))
#define  MAX(a,b) ((a) < (b) ? (b) : (a))

int scanNextField(CMDLINE *cmdline);
int scanNextWord(CMDLINE *cmdline);
int compareKeyword(CMDLINE *cmdline,const char *key);
void parseASMArgs(CMDLINE *cmdline, CMDARGS *arg);
FILE *openToFile(unsigned char tempbuf[]);
void asmExecuteCommand(FILE *fstdout, ASMDISKGROUP *dg, unsigned char *cmd);

#endif
