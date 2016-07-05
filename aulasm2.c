/*
*
*                                 Apache License
*                           Version 2.0, January 2004
*                        http://www.apache.org/licenses/
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "aulasm2.h"
#include "asmkernel.h"
#include <time.h>

#if defined(_WIN32)
#define STRNCASECMP memicmp
#else
#define STRNCASECMP strncasecmp
#endif

int scanNextField(CMDLINE *cmdline)
{
  unsigned int i=cmdline->epos;
  if (cmdline->epos > 0)
  {
    i = cmdline->epos + 1;
  }
  cmdline->spos = i;
  while(i<cmdline->length && cmdline->buf[i]!=',') i++;
  cmdline->epos = i;
  return cmdline->epos - cmdline->spos;
}

int scanNextWord(CMDLINE *cmdline)
{
  unsigned int i=cmdline->epos;
  while(i<cmdline->length && isspace(cmdline->buf[i])) i++;
  cmdline->spos = i;
  while(i<cmdline->length && !isspace(cmdline->buf[i])) i++;
  cmdline->epos = i;
  return cmdline->epos - cmdline->spos;
}

int compareKeyword(CMDLINE *cmdline,const char *key)
{
  if (cmdline->epos - cmdline->spos == strlen(key) &&
    STRNCASECMP(cmdline->buf+cmdline->spos,key,
      cmdline->epos - cmdline->spos) == 0)
  {
    return 0;
  }
  return 1;
}

int getASMCommand(FILE *cmdin,CMDLINE *cmdline)
{
    unsigned char cbuf[1024];
    unsigned int  lineno = 1;
    unsigned int  pos = 0;

    for (lineno = 1; ;)
    {
        if (lineno == 1)
        {
            memset(cmdline->buf,0x00,8192);
            fputs("ASM> ", stdout);
        }
        else
        {
            printf("%4d ", lineno);
        }

        memset(cbuf,0,1024);
        fgets((char *) cbuf,1024,cmdin);

        if (*cbuf == '\0')  continue;

        /* Concatenate to statement buffer. */
        if (lineno > 1)
            strcat((char *)cmdline->buf, " ");

        strcat((char *)cmdline->buf, (char *) cbuf);

        pos = strlen(cmdline->buf);

       while (pos > 0 && isspace(cmdline->buf[pos - 1]))
       {
         cmdline->buf[pos - 1] = '\0'; 
         pos --;
       }
       if (pos == 0) continue;

       if (cmdline->buf[pos - 1] == ';')
       {
            cmdline->buf[pos - 1] = '\0';
            pos --;
            cmdline->spos=0;
            cmdline->epos=0;
            cmdline->length=pos;
            break;
       }
       if (cmdline->buf[pos - 1] == '.')
       {
            cmdline->buf[0] = '\0';
            pos --;
            cmdline->spos=0;
            cmdline->epos=0;
            cmdline->length=0;
            break;
       }

       cmdline->spos=0;
       cmdline->epos=0;
       cmdline->length=pos;
       if (pos > 7168)
       {
         return 0;
       }
       if (scanNextWord(cmdline) > 0)
       {
          if (compareKeyword(cmdline,"EXIT") == 0) return -1;
          if (compareKeyword(cmdline,"QUIT") == 0) return -1;
          return 0;
       }
       else
       {
          continue;
       }
       lineno ++;
    }
    cmdline->length=strlen(cmdline->buf);
    return 0;
}

void setASMOptions(FILE *fstdout, CMDLINE *cmdline, ASMDISKGROUP *dg)
{
    unsigned char tempbuf[128];
    if (scanNextWord(cmdline))
    {
        if (compareKeyword(cmdline,"BLOCK_SIZE") == 0) 
        {
            if (scanNextWord(cmdline))
            {
               memset(tempbuf,0,128);
               memcpy(tempbuf,cmdline->buf+cmdline->spos,
                     cmdline->epos-cmdline->spos);
               dg->blksize = atoi(tempbuf);
            }
            fprintf(fstdout," BLOCK_SIZE = %d\n", dg->blksize);
        }
        else if (compareKeyword(cmdline,"ORACLE_BLOCK") == 0) 
        {
            if (scanNextWord(cmdline))
            {
               memset(tempbuf,0,128);
               memcpy(tempbuf,cmdline->buf+cmdline->spos,
                     cmdline->epos-cmdline->spos);
               dg->oblksize = atoi(tempbuf);
            }
            fprintf(fstdout," ORACLE_BLOCK = %d\n", dg->oblksize);
        }
        else if (compareKeyword(cmdline,"AU_SIZE") == 0) 
        {
            if (scanNextWord(cmdline))
            {
               memset(tempbuf,0,128);
               memcpy(tempbuf,cmdline->buf+cmdline->spos,
                     cmdline->epos-cmdline->spos);
               dg->ausize = atoi(tempbuf);
            }
            fprintf(fstdout," AU_SIZE = %d\n", dg->ausize);
        }
        else if (compareKeyword(cmdline,"F1B1_DISK") == 0) 
        {
            if (scanNextWord(cmdline))
            {
               memset(tempbuf,0,128);
               memcpy(tempbuf,cmdline->buf+cmdline->spos,
                     cmdline->epos-cmdline->spos);
               dg->f1b1disk = atoi(tempbuf);
               getASMSystemFileEntry(dg);
            }
            fprintf(fstdout," F1B1_DISK = %d\n", dg->f1b1disk);
        }
        else if (compareKeyword(cmdline,"F1B1_AU") == 0) 
        {
            if (scanNextWord(cmdline))
            {
               memset(tempbuf,0,128);
               memcpy(tempbuf,cmdline->buf+cmdline->spos,
                     cmdline->epos-cmdline->spos);
               dg->f1b1au = atoi(tempbuf);
               getASMSystemFileEntry(dg);
            }
            fprintf(fstdout," F1B1_AU = %d\n", dg->f1b1au);
        }
        else
        {
            fprintf(fstdout," Invalid option to set.\n");
        }
    }
    else
    {
       fprintf(fstdout," Valid options ORACLE_BLOCK BLOCK_SIZE, AU_SIZE, F1B1_DISK, F1B1_AU\n");
    }
}

int parseInteger(unsigned char tempbuf[])
{
    if (STRNCASECMP(tempbuf, "0X",2) == 0)
    {
       return strtol(tempbuf+2,NULL,16);
    }
    return atoi(tempbuf);
}

void parseASMArgs(CMDLINE *cmdline, CMDARGS *arg)
{
    unsigned char  tempbuf[256];
    int block_option=0;

    memset(arg->fname,0,512);
    arg->file = 0;
    arg->disk = 0;
    arg->aun  = 0;
    arg->block= 0;
    arg->level= 0;
    arg->count= 0;
    arg->offset=0;

    while(scanNextWord(cmdline) > 0)
    {
        if (compareKeyword(cmdline,"FILE") == 0)
           block_option = 5;
        else if (compareKeyword(cmdline,"DISK") == 0)
           block_option = 2;
        else if (compareKeyword(cmdline,"AU") == 0)
           block_option = 3;
        else if (compareKeyword(cmdline,"BLOCK") == 0)
           block_option = 4;
        else if (compareKeyword(cmdline,"LEVEL") == 0)
           block_option = 6;
        else if (compareKeyword(cmdline,"COUNT") == 0)
           block_option = 7;
        else if (compareKeyword(cmdline,"OFFSET") == 0)
           block_option = 8;
        else if (compareKeyword(cmdline,"TO") == 0)
           block_option = 1;
        else if (compareKeyword(cmdline,"PATH") == 0)
           block_option = 1;
        else
        {
           memset(tempbuf,0,256);
           memcpy(tempbuf,cmdline->buf+cmdline->spos,
             cmdline->epos - cmdline->spos);
           tempbuf[cmdline->epos - cmdline->spos]='\0';
           switch(block_option)
           {
             case 1:
               memcpy(arg->fname, cmdline->buf+cmdline->spos,
                       MIN(cmdline->epos - cmdline->spos,256));
               break;
             case 2:
               arg->disk = parseInteger(tempbuf);
               break;
             case 3:
               arg->aun = parseInteger(tempbuf);
               break;
             case 4:
               arg->block = parseInteger(tempbuf);
               break;
             case 5:
               arg->file = parseInteger(tempbuf);
               break;
             case 6:
               arg->level = parseInteger(tempbuf);
               break;
             case 7:
               arg->count = parseInteger(tempbuf);
               break;
             case 8:
               arg->offset = parseInteger(tempbuf);
               break;
           }
           block_option = 0;
       }
   }
}

void doASMHelp()
{
    printf("  SET        -- set options (BLOCK_SIZE, AU_SIZE, F1B1_DISK, F1B1_AU)\n");
    printf("  OPEN       -- open data file\n");
    printf("  ADD        -- add disk with customized disk num (disk, path)\n");
    printf("  LIST       -- list opened disks information (to)\n");
    printf("  LSAU       -- list file of given AU (disk, au, count)\n");
    printf("  FILE       -- list all file entries in ASM (file, [disk, au, block]).\n");
    printf("  ALIAS      -- list all alias entries in ASM (file, [disk, au, block]).\n");
    printf("  EXTENT     -- list or adjust variable extent (level, au, count).\n");
    printf("  FMAP       -- list file extent (file, [disk, au, block], to)\n");
    printf("  DUMP       -- dump file extent (file, [disk, au, block], offset, to)\n");
    printf("  COPY       -- copy file to os  (file, [disk, au, block], to)\n");
    printf("  QUIT/EXIT  -- exit the program.\n");
}

void asmExecuteCommand(FILE *fstdout, ASMDISKGROUP *dg, unsigned char *cmd)
{
    unsigned aui=0;
    CMDARGS args;
    CMDLINE cmdline;
    unsigned char tempbuf[512];
    ASMFILE afile;
    FILE *flog=NULL;
  
    memset(cmdline.buf,0,8192);
    cmdline.length = 0;
    if (cmd != NULL) 
    {
        cmdline.length = MIN(8191, strlen(cmd));
        memcpy(cmdline.buf, cmd, cmdline.length);
    }

    if (cmdline.length)
    {
        cmdline.spos=0;
        cmdline.epos=0;

        if (scanNextWord(&cmdline))
        {
            if (compareKeyword(&cmdline,"SET") == 0) 
            {
                setASMOptions(fstdout,&cmdline, dg);
                setDiskGroupCache(dg);
            }
            if (compareKeyword(&cmdline,"LSAU") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.count == 0) args.count = 1;
		printDate(fstdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   for(aui=args.aun; aui<args.aun + args.count; aui++)
                       listASMFileEntryAU(flog, dg, args.disk, aui);
		   fclose(flog);
                   flog = NULL;
                }
                else
                {
                   for(aui=args.aun; aui<args.aun + args.count; aui++)
                       listASMFileEntryAU(fstdout, dg, args.disk, aui);
                }
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"FILE") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.file && args.aun)
                {
                    dg->afile.fileid = args.file;
                    dg->afile.disk   = args.disk;
                    dg->afile.aun    = args.aun;
                    dg->afile.block  = args.block;
                }
		printDate(fstdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileEntry(flog, dg, &(dg->afile));
		   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileEntry(fstdout, dg, &(dg->afile));
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"ALIAS") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.file && args.aun)
                {
                    dg->alias.fileid = args.file;
                    dg->alias.disk   = args.disk;
                    dg->alias.aun    = args.aun;
                    dg->alias.block  = args.block;
                }
		printDate(fstdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMAliasEntry(flog, dg, &(dg->alias));
		   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMAliasEntry(fstdout, dg, &(dg->alias));
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"EXTENT") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                if (args.level && args.aun && args.count)
                {
                    setExtentAUCount(&(dg->policy), args.level, args.aun, args.count);
                }
                listASMExtentPolicy(fstdout, dg);
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"FMAP") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileExtent(flog, dg, &afile);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileExtent(fstdout, dg, &afile);
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"FMAP2") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileExtent2(flog, dg, &afile, args.offset);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileExtent2(fstdout, dg, &afile, args.offset);
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"DUMP") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   dumpASMFileBlock(flog, dg, &afile, args.offset);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   dumpASMFileBlock(fstdout, dg, &afile, args.offset);
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"COPY") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(dg, &afile, args.file);
                if (args.fname[0])
                {
                   if (readDiskGroupBlock(dg, afile.disk, afile.aun, afile.block) == dg->blksize)
                   {
                       if ((flog = fopen(args.fname,"wb+")) != NULL)
                       {
                           setbuf(flog, NULL);
                           copyASMFileOut(flog, dg, &afile);
                           fclose(flog);
                           flog = NULL;
                       }
                   }
                }
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"LIST") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(fstdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                    printDiskGroup(flog, dg);
                    fclose(flog);
                    flog = NULL;
                }
                else
                    printDiskGroup(fstdout, dg);
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"OPEN") == 0) 
            {
		printDate(fstdout);
		if (scanNextWord(&cmdline))
		{
		    memset(tempbuf,0,512);
                    memcpy(tempbuf, cmdline.buf+cmdline.spos,
                           MIN(cmdline.epos - cmdline.spos,511));
		    openDiskGroup(fstdout, dg, tempbuf);
		}
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"ADD") == 0) 
            {
                parseASMArgs(&cmdline, &args);
		printDate(fstdout);
		if (args.fname[0])
		{
		    openDiskGroupDisk(fstdout, dg, args.disk, args.fname);
		}
		printDate(fstdout);
            }
            if (compareKeyword(&cmdline,"HELP") == 0) 
            {
                doASMHelp();
            }
            fprintf(fstdout," \n");
            fprintf(fstdout," Unknown command, type HELP for help. \n");
            fprintf(fstdout," \n");
        }

    }
}

int runASM(FILE *cmdin)
{
    unsigned aui=0;
    CMDLINE cmdline;
    CMDARGS args;
    ASMDISKGROUP dg;
    unsigned char tempbuf[512];
    ASMFILE afile;
    FILE *flog=NULL;

    initASMDiskGroup(&dg);
    setDiskGroupCache(&dg);

    fprintf(stdout,"AUL : AnySQL UnLoader(MyDUL) for Oracle ASM, release 2.0.0\n");
    fputs("\n",stdout);
    fprintf(stdout,"(C) Copyright Lou Fangxin 2011-2012 (AnySQL.net), all rights reserved.\n");
    fputs("\n",stdout);
    
    while(getASMCommand(stdin,&cmdline) != -1)
    {
        cmdline.spos=0;
        cmdline.epos=0;
 
        if (scanNextWord(&cmdline))
        {
            if (compareKeyword(&cmdline,"SET") == 0) 
            {
                setASMOptions(stdout,&cmdline, &dg);
                setDiskGroupCache(&dg);
                continue;
            }
            if (compareKeyword(&cmdline,"LSAU") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.count == 0) args.count = 1;
		printDate(stdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   for(aui=args.aun; aui<args.aun + args.count; aui++)
                       listASMFileEntryAU(flog, &dg, args.disk, aui);
		   fclose(flog);
                   flog = NULL;
                }
                else
                {
                   for(aui=args.aun; aui<args.aun + args.count; aui++)
                       listASMFileEntryAU(stdout, &dg, args.disk, aui);
                }
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"FILE") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.file && args.aun)
                {
                    dg.afile.fileid = args.file;
                    dg.afile.disk   = args.disk;
                    dg.afile.aun    = args.aun;
                    dg.afile.block  = args.block;
                }
		printDate(stdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileEntry(flog, &dg, &(dg.afile));
		   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileEntry(stdout, &dg, &(dg.afile));
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"ALIAS") == 0) 
            {
		parseASMArgs(&cmdline, &args);
                if (args.file && args.aun)
                {
                    dg.alias.fileid = args.file;
                    dg.alias.disk   = args.disk;
                    dg.alias.aun    = args.aun;
                    dg.alias.block  = args.block;
                }
		printDate(stdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMAliasEntry(flog, &dg, &(dg.alias));
		   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMAliasEntry(stdout, &dg, &(dg.alias));
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"EXTENT") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                if (args.level && args.aun && args.count)
                {
                    setExtentAUCount(&(dg.policy), args.level, args.aun, args.count);
                }
                listASMExtentPolicy(stdout, &dg);
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"FMAP") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(&dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileExtent(flog, &dg, &afile);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileExtent(stdout, &dg, &afile);
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"FMAP2") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(&dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   listASMFileExtent2(flog, &dg, &afile, args.offset);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   listASMFileExtent2(stdout, &dg, &afile, args.offset);
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"DUMP") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(&dg, &afile, args.file);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                   dumpASMFileBlock(flog, &dg, &afile, args.offset);
                   fclose(flog);
                   flog = NULL;
                }
                else
                   dumpASMFileBlock(stdout, &dg, &afile, args.offset);
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"COPY") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                afile.fileid= args.file;
                afile.disk  = args.disk;
                afile.aun   = args.aun;
                afile.block = args.block;
                getASMFileEntry(&dg, &afile, args.file);
                if (args.fname[0])
                {
                   if (readDiskGroupBlock(&dg, afile.disk, afile.aun, afile.block) == dg.blksize)
                   {
                       if ((flog = fopen(args.fname,"wb+")) != NULL)
                       {
                           setbuf(flog, NULL);
                           copyASMFileOut(flog, &dg, &afile);
                           fclose(flog);
                           flog = NULL;
                       }
                   }
                }
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"LIST") == 0) 
            {
		parseASMArgs(&cmdline, &args);
		printDate(stdout);
                if (args.fname[0] && (flog = fopen(args.fname,"wb+")) != NULL)
                {
                    printDiskGroup(flog, &dg);
                    fclose(flog);
                    flog = NULL;
                }
                else
                    printDiskGroup(stdout, &dg);
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"OPEN") == 0) 
            {
		printDate(stdout);
		if (scanNextWord(&cmdline))
		{
		    memset(tempbuf,0,512);
                    memcpy(tempbuf, cmdline.buf+cmdline.spos,
                           MIN(cmdline.epos - cmdline.spos,511));
		    openDiskGroup(stdout, &dg, tempbuf);
		}
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"ADD") == 0) 
            {
                parseASMArgs(&cmdline, &args);
		printDate(stdout);
		if (args.fname[0])
		{
		    openDiskGroupDisk(stdout, &dg, args.disk, args.fname);
		}
		printDate(stdout);
                continue;
            }
            if (compareKeyword(&cmdline,"HELP") == 0) 
            {
                doASMHelp();
                continue;
            }
            printf(" \n");
            printf(" Unknown command, type HELP for help. \n");
            printf(" \n");
        }

    }
    closeDiskGroup(&dg);
    return 0;
}


int main()
{
  runASM(stdin);
}

