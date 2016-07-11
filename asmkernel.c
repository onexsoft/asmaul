/*
*
*                                 Apache License
*                           Version 2.0, January 2004
*                        http://www.apache.org/licenses/
*/

#include "asmkernel.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void  printDate(FILE *fp)
{
  time_t now = time(0);
  struct tm *ptm = localtime(&now);
  fprintf(fp,
    "%04d-%02d-%02d %02d:%02d:%02d\n",
    ptm->tm_year + 1900,
    ptm->tm_mon + 1,
    ptm->tm_mday,
    ptm->tm_hour,
    ptm->tm_min,
    ptm->tm_sec);
}

void  printBlock(FILE *fp,unsigned char *buf,size_t len)
{
  size_t i=0;
  unsigned char tempbuf[17];
  tempbuf[16]='\0';
  fprintf(fp,
    "     /0x  : -0-1 -2-3 -4-5 -6-7 -8-9 -a-b -c-d -e-f  0123456789abcdef\n");
  fprintf(fp,
    "-----/----: ---------------------------------------  ----------------\n");
  for(i=0;i<len;i++)
  {
    if (i % 16 == 0)
    {
      if (i > 0)
      {
        fprintf(fp,"  %s",tempbuf);
        fprintf(fp,"\n");
      }
      if (i > 0 && i % 256 == 0)
      {
        fprintf(fp,"\n");
        fprintf(fp,
          "     /0x  : -0-1 -2-3 -4-5 -6-7 -8-9 -a-b -c-d -e-f  0123456789abcdef\n");
        fprintf(fp,
          "-----/----: ---------------------------------------  ----------------\n");
      }
      fprintf(fp,"%05d/%04x:",i,i);
    }
    if (i % 2 == 0)
    {
      fprintf(fp," ");
    }
    fprintf(fp,"%x%x",(buf[i] >> 4 & 0x0f),(buf[i] & 0x0f));
    if (isprint(buf[i]))
      tempbuf[i%16] = buf[i];
    else
      tempbuf[i%16] = '.';
  }
  fprintf(fp,"  %s",tempbuf);
  fprintf(fp,"\n");
}

/*
  Internal function only
*/
void trimFileName(ub1 *fname)
{
   int i,len;
   len = strlen(fname);
   for(i=len-1;i>0;i--)
   {
       if (isspace(*(fname+i))) *(fname+i) = '\0';
       if (*(fname+i) == '\r') *(fname+i) = '\0';
       if (*(fname+i) == '\t') *(fname+i) = '\0';
   }
}

/*
kfbh.endian:                          1 ; 0x000: 0x01
kfbh.hard:                          130 ; 0x001: 0x82
kfbh.type:                            1 ; 0x002: KFBTYP_DISKHEAD
kfbh.datfmt:                          1 ; 0x003: 0x01
kfbh.block.blk:                       0 ; 0x004: T=0 NUMB=0x0
kfbh.block.obj:              2147483648 ; 0x008: TYPE=0x8 NUMB=0x0
kfbh.check:                  2146277315 ; 0x00c: 0x7fed97c3
kfbh.fcn.base:                       50 ; 0x010: 0x00000032
kfbh.fcn.wrap:                        0 ; 0x014: 0x00000000
kfbh.spare1:                          0 ; 0x018: 0x00000000
kfbh.spare2:                          0 ; 0x01c: 0x00000000
*/

void getASMBlockHeader(ASMBH *bh, ub1 buf[])
{
    memset(bh, 0, sizeof(ASMBH));

    bh->edian    = buf[0x000];
    bh->hard     = buf[0x001];
    bh->type     = buf[0x002];
    bh->datfmt   = buf[0x003];
    if (bh->edian == ASM_BIG_EDIAN)
    {
        bh->blockblk = BB2UB4(buf[0x004], buf[0x005], buf[0x006], buf[0x007]);
        bh->blockobj = BB2UB4(buf[0x008], buf[0x009], buf[0x00a], buf[0x00b]);
        bh->check    = BB2UB4(buf[0x00c], buf[0x00d], buf[0x00e], buf[0x00f]);
        bh->fcnbase  = BB2UB4(buf[0x010], buf[0x011], buf[0x012], buf[0x013]);
        bh->fcnwrap  = BB2UB4(buf[0x014], buf[0x015], buf[0x016], buf[0x017]);
        bh->spare1   = BB2UB4(buf[0x018], buf[0x019], buf[0x01a], buf[0x01b]);
        bh->spare2   = BB2UB4(buf[0x01c], buf[0x01d], buf[0x01e], buf[0x01f]);
    }
    else
    {
        bh->blockblk = LB2UB4(buf[0x004], buf[0x005], buf[0x006], buf[0x007]);
        bh->blockobj = LB2UB4(buf[0x008], buf[0x009], buf[0x00a], buf[0x00b]);
        bh->check    = LB2UB4(buf[0x00c], buf[0x00d], buf[0x00e], buf[0x00f]);
        bh->fcnbase  = LB2UB4(buf[0x010], buf[0x011], buf[0x012], buf[0x013]);
        bh->fcnwrap  = LB2UB4(buf[0x014], buf[0x015], buf[0x016], buf[0x017]);
        bh->spare1   = LB2UB4(buf[0x018], buf[0x019], buf[0x01a], buf[0x01b]);
        bh->spare2   = LB2UB4(buf[0x01c], buf[0x01d], buf[0x01e], buf[0x01f]);
    }
}

/*
Get following output with "kfed read xxx aun=0 blkn=0"

kfdhdb.driver.provstr:         ORCLDISK ; 0x000: length=8
kfdhdb.compat:                168820736 ; 0x020: 0x0a100000
kfdhdb.dsknum:                        0 ; 0x024: 0x0000
kfdhdb.grptyp:                        1 ; 0x026: KFDGTP_EXTERNAL
kfdhdb.hdrsts:                        3 ; 0x027: KFDHDR_MEMBER
kfdhdb.dskname:               TEST_0000 ; 0x028: length=9
kfdhdb.grpname:                    TEST ; 0x048: length=4
kfdhdb.fgname:                TEST_0000 ; 0x068: length=9
kfdhdb.capname:                         ; 0x088: length=0
kfdhdb.secsize:                     512 ; 0x0b8: 0x0200
kfdhdb.blksize:                    4096 ; 0x0ba: 0x1000
kfdhdb.ausize:                  1048576 ; 0x0bc: 0x00100000
kfdhdb.mfact:                    113792 ; 0x0c0: 0x0001bc80
kfdhdb.dsksize:                     200 ; 0x0c4: 0x000000c8
kfdhdb.pmcnt:                         2 ; 0x0c8: 0x00000002
kfdhdb.fstlocn:                       1 ; 0x0cc: 0x00000001
kfdhdb.altlocn:                       2 ; 0x0d0: 0x00000002
kfdhdb.f1b1locn:                      2 ; 0x0d4: 0x00000002
kfdhdb.dbcompat:              168820736 ; 0x0e0: 0x0a100000
*/

void getASMBlockMetaData(ASMMETA *bh, ub1 buf[])
{
    int _hsize=0x020;
    ub1 _edian=0;

    memset(bh, 0, sizeof(ASMMETA));
    _edian = buf[0];
    memcpy(bh->provstr, buf + _hsize + 0x000, 8);
    if (_edian == ASM_BIG_EDIAN)
    {
        bh->compat = BB2UB4(buf[_hsize + 0x020], buf[_hsize + 0x021], buf[_hsize + 0x022], buf[_hsize + 0x023]);
        bh->dsknum = BB2UB2(buf[_hsize + 0x024], buf[_hsize + 0x025]);
    }
    else
    {
        bh->compat = LB2UB4(buf[_hsize + 0x020], buf[_hsize + 0x021], buf[_hsize + 0x022], buf[_hsize + 0x023]);
        bh->dsknum = LB2UB2(buf[_hsize + 0x024], buf[_hsize + 0x025]);
    }
    bh->grptyp = buf[_hsize + 0x026];
    bh->hdrsts = buf[_hsize + 0x027];
    memcpy(bh->dskname, buf + _hsize + 0x028, 0x020);
    memcpy(bh->grpname, buf + _hsize + 0x048, 0x020);
    memcpy(bh->fgname , buf + _hsize + 0x068, 0x020);
    if (_edian == ASM_BIG_EDIAN)
    {
        bh->secsize  = BB2UB2(buf[_hsize + 0x0b8], buf[_hsize + 0x0b9]);
        bh->blksize  = BB2UB2(buf[_hsize + 0x0ba], buf[_hsize + 0x0bb]);
        bh->ausize   = BB2UB4(buf[_hsize + 0x0bc], buf[_hsize + 0x0bd], buf[_hsize + 0x0be], buf[_hsize + 0x0bf]);
        bh->mfact    = BB2UB4(buf[_hsize + 0x0c0], buf[_hsize + 0x0c1], buf[_hsize + 0x0c2], buf[_hsize + 0x0c3]);
        bh->dsksize  = BB2UB4(buf[_hsize + 0x0c4], buf[_hsize + 0x0c5], buf[_hsize + 0x0c6], buf[_hsize + 0x0c7]);
        bh->pmcnt    = BB2UB4(buf[_hsize + 0x0c8], buf[_hsize + 0x0c9], buf[_hsize + 0x0ca], buf[_hsize + 0x0cb]);
        bh->fstlocn  = BB2UB4(buf[_hsize + 0x0cc], buf[_hsize + 0x0cd], buf[_hsize + 0x0ce], buf[_hsize + 0x0cf]);
        bh->altlocn  = BB2UB4(buf[_hsize + 0x0d0], buf[_hsize + 0x0d1], buf[_hsize + 0x0d2], buf[_hsize + 0x0d3]);
        bh->f1b1locn = BB2UB4(buf[_hsize + 0x0d4], buf[_hsize + 0x0d5], buf[_hsize + 0x0d6], buf[_hsize + 0x0d7]);
        bh->dbcompat = BB2UB4(buf[_hsize + 0x0e0], buf[_hsize + 0x0e1], buf[_hsize + 0x0e2], buf[_hsize + 0x0e3]);
    }
    else
    {
        bh->secsize  = LB2UB2(buf[_hsize + 0x0b8], buf[_hsize + 0x0b9]);
        bh->blksize  = LB2UB2(buf[_hsize + 0x0ba], buf[_hsize + 0x0bb]);
        bh->ausize   = LB2UB4(buf[_hsize + 0x0bc], buf[_hsize + 0x0bd], buf[_hsize + 0x0be], buf[_hsize + 0x0bf]);
        bh->mfact    = LB2UB4(buf[_hsize + 0x0c0], buf[_hsize + 0x0c1], buf[_hsize + 0x0c2], buf[_hsize + 0x0c3]);
        bh->dsksize  = LB2UB4(buf[_hsize + 0x0c4], buf[_hsize + 0x0c5], buf[_hsize + 0x0c6], buf[_hsize + 0x0c7]);
        bh->pmcnt    = LB2UB4(buf[_hsize + 0x0c8], buf[_hsize + 0x0c9], buf[_hsize + 0x0ca], buf[_hsize + 0x0cb]);
        bh->fstlocn  = LB2UB4(buf[_hsize + 0x0cc], buf[_hsize + 0x0cd], buf[_hsize + 0x0ce], buf[_hsize + 0x0cf]);
        bh->altlocn  = LB2UB4(buf[_hsize + 0x0d0], buf[_hsize + 0x0d1], buf[_hsize + 0x0d2], buf[_hsize + 0x0d3]);
        bh->f1b1locn = LB2UB4(buf[_hsize + 0x0d4], buf[_hsize + 0x0d5], buf[_hsize + 0x0d6], buf[_hsize + 0x0d7]);
        bh->dbcompat = LB2UB4(buf[_hsize + 0x0e0], buf[_hsize + 0x0e1], buf[_hsize + 0x0e2], buf[_hsize + 0x0e3]);
    }
}

/*
kfffdb.hibytes:                       0 ; 0x00c: 0x00000000
kfffdb.lobytes:                 2097152 ; 0x010: 0x00200000
kfffdb.xtntcnt:                       2 ; 0x014: 0x00000002
kfffdb.xtnteof:                       2 ; 0x018: 0x00000002
kfffdb.blkSize:                    4096 ; 0x01c: 0x00001000
kfffdb.flags:                        65 ; 0x020: O=1 S=0 S=0 D=0 C=0 I=0 R=1 A=0
kfffdb.fileType:                     15 ; 0x021: 0x0f
kfffdb.dXrs:                         17 ; 0x022: SCHE=0x1 NUMB=0x1
kfffdb.iXrs:                         17 ; 0x023: SCHE=0x1 NUMB=0x1
kfffdb.dXsiz[0]:             4294967295 ; 0x024: 0xffffffff
kfffdb.dXsiz[1]:                      0 ; 0x028: 0x00000000
kfffdb.dXsiz[2]:                      0 ; 0x02c: 0x00000000
kfffdb.iXsiz[0]:             4294967295 ; 0x030: 0xffffffff
kfffdb.iXsiz[1]:                      0 ; 0x034: 0x00000000
kfffdb.iXsiz[2]:                      0 ; 0x038: 0x00000000
kfffdb.xtntblk:                       2 ; 0x03c: 0x0002
kfffdb.break:                        60 ; 0x03e: 0x003c
kfffdb.priZn:                         0 ; 0x040: KFDZN_COLD
kfffdb.secZn:                         0 ; 0x041: KFDZN_COLD
kfffdb.alias[0]:             4294967295 ; 0x044: 0xffffffff
kfffdb.alias[1]:             4294967295 ; 0x048: 0xffffffff
kfffdb.strpwdth:                      0 ; 0x04c: 0x00
kfffdb.strpsz:                        0 ; 0x04d: 0x00
kfffdb.usmsz:                         0 ; 0x04e: 0x0000
*/

void getASMFileHeader(ASMFH *bh, ub1 buf[])
{
    int _hsize=0x020;
    ub1 _edian=0;

    memset(bh, 0, sizeof(ASMFH));
    _edian = buf[0];

    if (_edian == ASM_BIG_EDIAN)
    {
        bh->hibytes = BB2UB4(buf[_hsize + 0x00c], buf[_hsize + 0x00d], buf[_hsize + 0x00e], buf[_hsize + 0x00f]);
        bh->lobytes = BB2UB4(buf[_hsize + 0x010], buf[_hsize + 0x011], buf[_hsize + 0x012], buf[_hsize + 0x013]);
        bh->xtntcnt = BB2UB4(buf[_hsize + 0x014], buf[_hsize + 0x015], buf[_hsize + 0x016], buf[_hsize + 0x017]);
        bh->xtnteof = BB2UB4(buf[_hsize + 0x018], buf[_hsize + 0x019], buf[_hsize + 0x01a], buf[_hsize + 0x01b]);
        bh->blksize = BB2UB4(buf[_hsize + 0x01c], buf[_hsize + 0x01d], buf[_hsize + 0x01e], buf[_hsize + 0x01f]);
    }
    else
    {
        bh->hibytes = LB2UB4(buf[_hsize + 0x00c], buf[_hsize + 0x00d], buf[_hsize + 0x00e], buf[_hsize + 0x00f]);
        bh->lobytes = LB2UB4(buf[_hsize + 0x010], buf[_hsize + 0x011], buf[_hsize + 0x012], buf[_hsize + 0x013]);
        bh->xtntcnt = LB2UB4(buf[_hsize + 0x014], buf[_hsize + 0x015], buf[_hsize + 0x016], buf[_hsize + 0x017]);
        bh->xtnteof = LB2UB4(buf[_hsize + 0x018], buf[_hsize + 0x019], buf[_hsize + 0x01a], buf[_hsize + 0x01b]);
        bh->blksize = LB2UB4(buf[_hsize + 0x01c], buf[_hsize + 0x01d], buf[_hsize + 0x01e], buf[_hsize + 0x01f]);
    }
    bh->flags   = buf[_hsize + 0x020];
    bh->filetype= buf[_hsize + 0x021];
    bh->dxrs    = buf[_hsize + 0x022];
    bh->ixrs    = buf[_hsize + 0x023];
    if (_edian == ASM_BIG_EDIAN)
    {
        bh->dxsiz0  = BB2UB4(buf[_hsize + 0x024], buf[_hsize + 0x025], buf[_hsize + 0x026], buf[_hsize + 0x027]);
        bh->dxsiz1  = BB2UB4(buf[_hsize + 0x028], buf[_hsize + 0x029], buf[_hsize + 0x02a], buf[_hsize + 0x02b]);
        bh->dxsiz2  = BB2UB4(buf[_hsize + 0x02c], buf[_hsize + 0x02d], buf[_hsize + 0x02e], buf[_hsize + 0x02f]);
        bh->ixsiz0  = BB2UB4(buf[_hsize + 0x030], buf[_hsize + 0x031], buf[_hsize + 0x032], buf[_hsize + 0x033]);
        bh->ixsiz1  = BB2UB4(buf[_hsize + 0x034], buf[_hsize + 0x035], buf[_hsize + 0x036], buf[_hsize + 0x037]);
        bh->ixsiz2  = BB2UB4(buf[_hsize + 0x038], buf[_hsize + 0x039], buf[_hsize + 0x03a], buf[_hsize + 0x03b]);
        bh->xtntblk = BB2UB2(buf[_hsize + 0x03c], buf[_hsize + 0x03d]);
        bh->xbreak  = BB2UB2(buf[_hsize + 0x03e], buf[_hsize + 0x03f]);
    }
    else
    {
        bh->dxsiz0  = LB2UB4(buf[_hsize + 0x024], buf[_hsize + 0x025], buf[_hsize + 0x026], buf[_hsize + 0x027]);
        bh->dxsiz1  = LB2UB4(buf[_hsize + 0x028], buf[_hsize + 0x029], buf[_hsize + 0x02a], buf[_hsize + 0x02b]);
        bh->dxsiz2  = LB2UB4(buf[_hsize + 0x02c], buf[_hsize + 0x02d], buf[_hsize + 0x02e], buf[_hsize + 0x02f]);
        bh->ixsiz0  = LB2UB4(buf[_hsize + 0x030], buf[_hsize + 0x031], buf[_hsize + 0x032], buf[_hsize + 0x033]);
        bh->ixsiz1  = LB2UB4(buf[_hsize + 0x034], buf[_hsize + 0x035], buf[_hsize + 0x036], buf[_hsize + 0x037]);
        bh->ixsiz2  = LB2UB4(buf[_hsize + 0x038], buf[_hsize + 0x039], buf[_hsize + 0x03a], buf[_hsize + 0x03b]);
        bh->xtntblk = LB2UB2(buf[_hsize + 0x03c], buf[_hsize + 0x03d]);
        bh->xbreak  = LB2UB2(buf[_hsize + 0x03e], buf[_hsize + 0x03f]);
    }
    bh->prizn = buf[_hsize + 0x040];
    bh->seczn = buf[_hsize + 0x041];
    if (_edian == ASM_BIG_EDIAN)
    {
        bh->alias0  = BB2UB4(buf[_hsize + 0x044], buf[_hsize + 0x045], buf[_hsize + 0x046], buf[_hsize + 0x047]);
        bh->alias1  = BB2UB4(buf[_hsize + 0x048], buf[_hsize + 0x049], buf[_hsize + 0x04a], buf[_hsize + 0x04b]);
    }
    else
    {
        bh->alias0  = LB2UB4(buf[_hsize + 0x044], buf[_hsize + 0x045], buf[_hsize + 0x046], buf[_hsize + 0x047]);
        bh->alias1  = LB2UB4(buf[_hsize + 0x048], buf[_hsize + 0x049], buf[_hsize + 0x04a], buf[_hsize + 0x04b]);
    }
    bh->strpwdth = buf[_hsize + 0x04c];
    bh->strpsz   = buf[_hsize + 0x04d];
    if (_edian == ASM_BIG_EDIAN)
    {
        bh->usmsz  = BB2UB2(buf[_hsize + 0x04e], buf[_hsize + 0x04f]);
    }
    else
    {
        bh->usmsz  = LB2UB2(buf[_hsize + 0x04e], buf[_hsize + 0x04f]);
    }
}

/*
only valid for block type = ASM_BLOCK_INDIRECT

kffixb.dxsn:                         60 ; 0x000: 0x0000003c
kffixb.xtntblk:                      69 ; 0x004: 0x0045
kffixb.dXrs:                         17 ; 0x006: SCHE=0x1 NUMB=0x1
kffixb.ub1spare:                      0 ; 0x007: 0x00
kffixb.ub4spare:                      0 ; 0x008: 0x00000000
*/

void getASMExtentHeader(ASMEXTH *bh,  ub1 buf[])
{
    int _hsize=0x020;
    ub1 _edian=0;

    memset(bh, 0, sizeof(ASMEXTH));
    _edian = buf[0];

   if (_edian == ASM_BIG_EDIAN)
   {
       bh->dxsn    = BB2UB4(buf[_hsize + 0x000], buf[_hsize + 0x001], buf[_hsize + 0x002], buf[_hsize + 0x003]);
       bh->xtntblk = BB2UB2(buf[_hsize + 0x004], buf[_hsize + 0x005]);
   }
   else
   {
       bh->dxsn    = LB2UB4(buf[_hsize + 0x000], buf[_hsize + 0x001], buf[_hsize + 0x002], buf[_hsize + 0x003]);
       bh->xtntblk = LB2UB2(buf[_hsize + 0x004], buf[_hsize + 0x005]);
   }
   bh->dxrs = buf[_hsize + 0x006];
}

/*
if block type is ASM_BLOCK_FILEDIR, offset = 0x020(BH) + 0x4a0;
if block type is ASM_BLOCK_INDIRECT, offset = 0x020(BH) + 0x00c;

kffixe[0].xptr.au:                   58 ; 0x00c: 0x0000003a
kffixe[0].xptr.disk:                  0 ; 0x010: 0x0000
kffixe[0].xptr.flags:                 0 ; 0x012: L=0 E=0 D=0 S=0
kffixe[0].xptr.chk:                  16 ; 0x013: 0x10
*/

void getASMFileExtent(ASMEXTENT *bh, ub1 buf[], ub2 buflen, ub2 xid)
{
    ASMBH bufh;
    ub2 _hsize = 0;

    memset(bh,0,sizeof(ASMEXTENT));
    getASMBlockHeader(&bufh, buf);

    if (bufh.type == ASM_BLOCK_FILEDIR)
    {
        _hsize = 0x020 + 0x4a0;
    }
    if (bufh.type == ASM_BLOCK_INDIRECT) 
    {
        _hsize = 0x020 + 0x00c;
    }    
    if (bufh.type != ASM_BLOCK_FILEDIR && bufh.type != ASM_BLOCK_INDIRECT) return;

    if ((_hsize + (xid + 1) * sizeof(ASMEXTENT)) > buflen) return;

    _hsize = _hsize + xid * sizeof(ASMEXTENT);

    if (bufh.edian == ASM_BIG_EDIAN)
    {
    	bh->au    = BB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->disk  = BB2UB2(buf[_hsize + 0x04], buf[_hsize + 0x05]);
    }
    else
    {
    	bh->au    = LB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->disk  = LB2UB2(buf[_hsize + 0x04], buf[_hsize + 0x05]);    	
    }
    bh->flags = buf[_hsize + 0x06];
    bh->chk   = buf[_hsize + 0x07];
}

/*
kffdnd.bnode.incarn:                  1 ; 0x000: A=1 NUMM=0x0
kffdnd.bnode.frlist.number:  4294967295 ; 0x004: 0xffffffff
kffdnd.bnode.frlist.incarn:           0 ; 0x008: A=0 NUMM=0x0
kffdnd.overfl.number:        4294967295 ; 0x00c: 0xffffffff
kffdnd.overfl.incarn:                 0 ; 0x010: A=0 NUMM=0x0
kffdnd.parent.number:                 0 ; 0x014: 0x00000000
kffdnd.parent.incarn:                 1 ; 0x018: A=1 NUMM=0x0
kffdnd.fstblk.number:                 2 ; 0x01c: 0x00000002
kffdnd.fstblk.incarn:                 1 ; 0x020: A=1 NUMM=0x0
*/
void getASMAliasHeader(ASMAH *bh, ub1 buf[])
{
    int _hsize=0x020;
    ub1 _edian=0;

    memset(bh, 0, sizeof(ASMAH));
    _edian = buf[0];
    if (_edian == ASM_BIG_EDIAN)
    {
    	bh->bnode_incarn    = BB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->frlist_number   = BB2UB4(buf[_hsize + 0x04], buf[_hsize + 0x05], buf[_hsize + 0x06], buf[_hsize + 0x07]); 
        bh->frlist_incarn   = BB2UB4(buf[_hsize + 0x08], buf[_hsize + 0x09], buf[_hsize + 0x0a], buf[_hsize + 0x0b]); 
        bh->overfl_number   = BB2UB4(buf[_hsize + 0x0c], buf[_hsize + 0x0d], buf[_hsize + 0x0e], buf[_hsize + 0x0f]); 
        bh->overfl_incarn   = BB2UB4(buf[_hsize + 0x10], buf[_hsize + 0x11], buf[_hsize + 0x12], buf[_hsize + 0x13]); 
        bh->parent_number   = BB2UB4(buf[_hsize + 0x14], buf[_hsize + 0x15], buf[_hsize + 0x16], buf[_hsize + 0x17]); 
        bh->parent_incarn   = BB2UB4(buf[_hsize + 0x18], buf[_hsize + 0x19], buf[_hsize + 0x1a], buf[_hsize + 0x1b]); 
        bh->fstblk_number   = BB2UB4(buf[_hsize + 0x1c], buf[_hsize + 0x1d], buf[_hsize + 0x1e], buf[_hsize + 0x1f]); 
        bh->fstblk_incarn   = BB2UB4(buf[_hsize + 0x20], buf[_hsize + 0x21], buf[_hsize + 0x22], buf[_hsize + 0x23]); 
    }
    else
    {
    	bh->bnode_incarn    = LB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->frlist_number   = LB2UB4(buf[_hsize + 0x04], buf[_hsize + 0x05], buf[_hsize + 0x06], buf[_hsize + 0x07]); 
        bh->frlist_incarn   = LB2UB4(buf[_hsize + 0x08], buf[_hsize + 0x09], buf[_hsize + 0x0a], buf[_hsize + 0x0b]); 
        bh->overfl_number   = LB2UB4(buf[_hsize + 0x0c], buf[_hsize + 0x0d], buf[_hsize + 0x0e], buf[_hsize + 0x0f]); 
        bh->overfl_incarn   = LB2UB4(buf[_hsize + 0x10], buf[_hsize + 0x11], buf[_hsize + 0x12], buf[_hsize + 0x13]); 
        bh->parent_number   = LB2UB4(buf[_hsize + 0x14], buf[_hsize + 0x15], buf[_hsize + 0x16], buf[_hsize + 0x17]); 
        bh->parent_incarn   = LB2UB4(buf[_hsize + 0x18], buf[_hsize + 0x19], buf[_hsize + 0x1a], buf[_hsize + 0x1b]); 
        bh->fstblk_number   = LB2UB4(buf[_hsize + 0x1c], buf[_hsize + 0x1d], buf[_hsize + 0x1e], buf[_hsize + 0x1f]); 
        bh->fstblk_incarn   = LB2UB4(buf[_hsize + 0x20], buf[_hsize + 0x21], buf[_hsize + 0x22], buf[_hsize + 0x23]); 
    }
}

/*
kfade[0].entry.incarn:                1 ; 0x024: A=1 NUMM=0x0
kfade[0].entry.hash:          710518681 ; 0x028: 0x2a59a799
kfade[0].entry.refer.number:          3 ; 0x02c: 0x00000003
kfade[0].entry.refer.incarn:          1 ; 0x030: A=1 NUMM=0x0
kfade[0].name:                 DATAFILE ; 0x034: length=8
kfade[0].fnum:               4294967295 ; 0x064: 0xffffffff
kfade[0].finc:               4294967295 ; 0x068: 0xffffffff
kfade[0].flags:                       4 ; 0x06c: U=0 S=0 S=1 U=0 F=0
kfade[0].ub1spare:                    0 ; 0x06d: 0x00
kfade[0].ub2spare:                    0 ; 0x06e: 0x0000
*/
void getASMAliasRecord(ASMALIAS *bh, ub1 buf[], ub2 buflen, ub2 xid)
{
    int _hsize=0x020;
    ub1 _edian=0;
    ASMBH bufh;

    getASMBlockHeader(&bufh, buf);
    _hsize = _hsize + 0x24;
    memset(bh, 0, sizeof(ASMALIAS));
    _edian = buf[0];

    if (bufh.type != ASM_BLOCK_ALIASDIR) return;

    if ((_hsize + (xid + 1) * sizeof(ASMALIAS)) > buflen) return;

    _hsize = _hsize + xid * sizeof(ASMALIAS);

    memcpy(bh->name, buf + _hsize + 0x10, 0x30);
    bh->flags        = buf[_hsize + 0x48];
    bh->ub1spare     = buf[_hsize + 0x49];
    if (bufh.edian == ASM_BIG_EDIAN)
    {
    	bh->entry_incarn = BB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->entry_hash   = BB2UB4(buf[_hsize + 0x04], buf[_hsize + 0x05], buf[_hsize + 0x06], buf[_hsize + 0x07]);
        bh->refer_number = BB2UB4(buf[_hsize + 0x08], buf[_hsize + 0x09], buf[_hsize + 0x0a], buf[_hsize + 0x0b]);
        bh->refer_incarn = BB2UB4(buf[_hsize + 0x0c], buf[_hsize + 0x0d], buf[_hsize + 0x0e], buf[_hsize + 0x0f]); 
        bh->fnum         = BB2UB4(buf[_hsize + 0x40], buf[_hsize + 0x41], buf[_hsize + 0x42], buf[_hsize + 0x43]); 
        bh->finc         = BB2UB4(buf[_hsize + 0x44], buf[_hsize + 0x45], buf[_hsize + 0x46], buf[_hsize + 0x47]); 
        bh->ub2spare     = BB2UB2(buf[_hsize + 0x4a], buf[_hsize + 0x4b]); 
    }
    else
    {
    	bh->entry_incarn = LB2UB4(buf[_hsize + 0x00], buf[_hsize + 0x01], buf[_hsize + 0x02], buf[_hsize + 0x03]);
        bh->entry_hash   = LB2UB4(buf[_hsize + 0x04], buf[_hsize + 0x05], buf[_hsize + 0x06], buf[_hsize + 0x07]);
        bh->refer_number = LB2UB4(buf[_hsize + 0x08], buf[_hsize + 0x09], buf[_hsize + 0x0a], buf[_hsize + 0x0b]);
        bh->refer_incarn = LB2UB4(buf[_hsize + 0x0c], buf[_hsize + 0x0d], buf[_hsize + 0x0e], buf[_hsize + 0x0f]); 
        bh->fnum         = LB2UB4(buf[_hsize + 0x40], buf[_hsize + 0x41], buf[_hsize + 0x42], buf[_hsize + 0x43]); 
        bh->finc         = LB2UB4(buf[_hsize + 0x44], buf[_hsize + 0x45], buf[_hsize + 0x46], buf[_hsize + 0x47]); 
        bh->ub2spare     = LB2UB2(buf[_hsize + 0x4a], buf[_hsize + 0x4b]); 
    }
}


void initASMDisk(ASMDISK *disk)
{
    disk->dsknum=0xffff;
    disk->dsksize=0xffffffff;
    disk->ausize = 0;
    disk->blksize= 0;
    memset(disk->dskname,0,32);
    memset(disk->grpname,0,32);
    disk->fp=NULL;
}

void initASMDiskGroup(ASMDISKGROUP *dg)
{
    int i=0;
    dg->compat   = 0;
    dg->dskcnt   = 0;
    dg->ausize   = 1048576;
    dg->blksize  = 4096;
    dg->oblksize = 8192;
    dg->f1b1disk = 0xffff;
    dg->f1b1au   = 0xffffffff;
    dg->cache.blk_disk = 0xffff;
    dg->cache.blk_aun  = 0xffffff;
    dg->cache.blk_block= 0xffff;
    dg->cache.blkbuf=NULL;
    dg->cache.au_disk = 0xffff;
    dg->cache.au_aun  = 0xffffff;
    dg->cache.aubuf =NULL;
    dg->cache.ex_disk = 0xffff;
    dg->cache.ex_aun  = 0xffffff;
    dg->cache.exbuf =NULL;
    for(i=0;i<65536;i++)
        dg->index[i] = 0xffff;
    for(i=0;i<ASM_MAX_DISKS;i++)
        initASMDisk(&(dg->disks[i]));
    dg->afile.fileid = 0xffffffff;
    dg->afile.disk   = 0xffff;
    dg->afile.aun    = 0xffffffff;
    dg->afile.block  = 0;
    dg->alias.fileid = 0xffffffff;
    dg->alias.disk   = 0xffff;
    dg->alias.aun    = 0xffffffff;
    dg->alias.block  = 0;
    dg->adisk.fileid = 0xffffffff;
    dg->adisk.disk   = 0xffff;
    dg->adisk.aun    = 0xffffffff;
    dg->adisk.block  = 0;
    
    dg->policy.level1au = 0xffffffff;
    dg->policy.level1sz = 0x0001;
    dg->policy.level2au = 0xffffffff;
    dg->policy.level2sz = 0x0001;
    dg->policy.level3au = 0xffffffff;
    dg->policy.level3sz = 0x0001;
    dg->policy.level4au = 0xffffffff;
    dg->policy.level4sz = 0x0001;
}

ub4 readFromDisk(ASMDISK *dsk, ub8 offset, ub1 buf[], ub4 buflen)
{
    fpos_t fpos;

    if (dsk->fp != NULL)
    {
       #if defined(SIMPLE_FPOS_T)
       fpos = offset;
       #else
       fpos.__pos = offset;
       #endif
       if (fsetpos(dsk->fp,&fpos) == 0)
       {
	   if (fread(buf,buflen,1,dsk->fp) == 1) return buflen;
       }
    }
    return 0;
}

void closeDiskGroup(ASMDISKGROUP *dg)
{
    int i=0;
    dg->compat   = 0;
    dg->dskcnt   = 0;
    dg->f1b1disk = 0xffff;
    dg->f1b1au   = 0xffffffff;
    for(i=0;i<65536;i++)
        dg->index[i] = 0xffff;
    for(i=0;i<dg->dskcnt;i++)
    {
        if (dg->disks[i].fp != NULL)
        {
            fclose(dg->disks[i].fp);
            dg->disks[i].fp = NULL;
        }
    }
    for(i=0;i<ASM_MAX_DISKS;i++)
        initASMDisk(&(dg->disks[i]));
    if (dg->cache.aubuf != NULL) free(dg->cache.aubuf);
    if (dg->cache.blkbuf != NULL) free(dg->cache.blkbuf);
    if (dg->cache.exbuf != NULL) free(dg->cache.exbuf);
    dg->cache.aubuf  = NULL;
    dg->cache.blkbuf = NULL;
    dg->cache.exbuf  = NULL;
    dg->afile.fileid = 0xffffffff;
    dg->afile.disk   = 0xffff;
    dg->afile.aun    = 0xffffffff;
    dg->afile.block  = 0;
    dg->alias.fileid = 0xffffffff;
    dg->alias.disk   = 0xffff;
    dg->alias.aun    = 0xffffffff;
    dg->alias.block  = 0;
    dg->adisk.fileid = 0xffffffff;
    dg->adisk.disk   = 0xffff;
    dg->adisk.aun    = 0xffffffff;
    dg->adisk.block  = 0;

    dg->policy.level1au = 0xffffffff;
    dg->policy.level1sz = 0x0001;
    dg->policy.level2au = 0xffffffff;
    dg->policy.level2sz = 0x0001;
    dg->policy.level3au = 0xffffffff;
    dg->policy.level3sz = 0x0001;
    dg->policy.level4au = 0xffffffff;
    dg->policy.level4sz = 0x0001;
}

void openDiskGroup(FILE *flog, ASMDISKGROUP *dg, ub1 *fname)
{
    ub1  dskvalid = 0x00;
    FILE *fp=NULL;
    char buf[256];
    ASMBH   bhead;
    ASMMETA bmeta;

    closeDiskGroup(dg);
    setDiskGroupCache(dg);

    if ((fp = fopen(fname,"r+")) == NULL)
    {
       if (flog != NULL) fprintf(flog, "Cannot open disk list file (%s)!\n", fname);
       return;
    }

    while (dg->dskcnt < ASM_MAX_DISKS && !feof(fp))
    {
       memset(buf,0,256);
       fgets(buf,255,fp);  
       if (buf[0] && buf[0] != '#')
       {
	  trimFileName(buf);
          if ((dg->disks[dg->dskcnt].fp = fopen(buf, "rb")) != NULL)
	  {
              dskvalid = 0x00;
              setbuf(dg->disks[dg->dskcnt].fp, NULL);
              if (readFromDisk(&(dg->disks[dg->dskcnt]),0,dg->cache.blkbuf,dg->blksize) == dg->blksize)
	      {
                  getASMBlockHeader(&bhead, dg->cache.blkbuf);
		  if (bhead.type == ASM_BLOCK_DISKHEAD)
		  {
		      getASMBlockMetaData(&bmeta, dg->cache.blkbuf);
		      dg->disks[dg->dskcnt].ausize  = bmeta.ausize;
		      dg->disks[dg->dskcnt].blksize = bmeta.blksize; 
                      dg->disks[dg->dskcnt].dsknum  = bmeta.dsknum;
		      dg->disks[dg->dskcnt].dsksize = bmeta.dsksize;
		      memcpy(dg->disks[dg->dskcnt].dskname, bmeta.dskname, 32);
		      memcpy(dg->disks[dg->dskcnt].grpname, bmeta.grpname, 32);
                      memcpy(dg->disks[dg->dskcnt].path, buf, 256);
		      dg->index[bmeta.dsknum] = dg->dskcnt;
                      dg->compat = bmeta.compat;
		      if (bmeta.f1b1locn)
		      {
                          dg->f1b1disk = bmeta.dsknum;
                          dg->f1b1au   = bmeta.f1b1locn;
                      }
		      dg->dskcnt ++;
                      dskvalid = 0x01;
                  }
              }
              if (!dskvalid)
              {
                  fclose(dg->disks[dg->dskcnt].fp);
		  dg->disks[dg->dskcnt].fp = NULL;
		  if (flog != NULL) fprintf(flog, "Invalid Disk : %s\n", buf);
              }
          }
       }
    }
    if (dg->dskcnt && dg->f1b1disk < 0xffff && dg->f1b1au < 0xffffffff)
    {
        getASMSystemFileEntry(dg);
    }
}

void openDiskGroupDisk(FILE *flog, ASMDISKGROUP *dg, ub2 disk, ub1 *fname)
{
    ASMBH   bhead;
    ASMMETA bmeta;

    if (dg->dskcnt < ASM_MAX_DISKS && (dg->disks[dg->dskcnt].fp = fopen(fname, "rb")) != NULL)
    {
        setbuf(dg->disks[dg->dskcnt].fp, NULL);
        if (readFromDisk(&(dg->disks[dg->dskcnt]),0,dg->cache.blkbuf,dg->blksize) == dg->blksize)
        {
                  getASMBlockHeader(&bhead, dg->cache.blkbuf);
		  if (bhead.type == ASM_BLOCK_DISKHEAD)
		  {
		      getASMBlockMetaData(&bmeta, dg->cache.blkbuf);
		      dg->disks[dg->dskcnt].ausize  = bmeta.ausize;
		      dg->disks[dg->dskcnt].blksize = bmeta.blksize; 
                      dg->disks[dg->dskcnt].dsknum  = bmeta.dsknum;
		      dg->disks[dg->dskcnt].dsksize = bmeta.dsksize;
		      memcpy(dg->disks[dg->dskcnt].dskname, bmeta.dskname, 32);
		      memcpy(dg->disks[dg->dskcnt].grpname, bmeta.grpname, 32);
                      memcpy(dg->disks[dg->dskcnt].path, fname, MIN(255,strlen(fname)));
                      dg->compat = bmeta.compat;
		      if (bmeta.f1b1locn < 0xffffffff)
		      {
                          dg->f1b1disk = bmeta.dsknum;
                          dg->f1b1au   = bmeta.f1b1locn;
                      }
                  }

        }
        dg->disks[dg->dskcnt].dsknum = disk;
        dg->index[disk] = dg->dskcnt;
        dg->dskcnt ++;
    }
    if (dg->dskcnt && dg->f1b1disk < 0xffff && dg->f1b1au < 0xffffffff)
    {
        getASMSystemFileEntry(dg);
    }
}


void printDiskGroup(FILE *flog, ASMDISKGROUP *dg)
{
     int i=0;
     
     if (flog != NULL && dg->dskcnt)
     {
         fprintf(flog, "Total Disks = %d, ausize=%d, blksize=%d\n", dg->dskcnt, dg->ausize, dg->blksize);
         fprintf(flog, "\n");
         fprintf(flog, " disk     size block disk name        disk group   disk path               \n");
         fprintf(flog, "===== ======== ===== ================ ============ ========================\n");
         for(i=0;i<dg->dskcnt;i++)
         {
             fprintf(flog, "%5d %8d %5d %-*s %-*s %s\n", 
                  dg->disks[i].dsknum, dg->disks[i].dsksize, dg->disks[i].blksize,
                  16, dg->disks[i].dskname, 12, dg->disks[i].grpname, dg->disks[i].path );
         }
         fprintf(flog, "\n");
         fprintf(flog, "f1b1disk = %d, f1b1au = %d, score=%d, compat=0x%08x\n", 
                 dg->f1b1disk, dg->f1b1au, scoreASMFileEntryAU(dg, dg->f1b1disk, dg->f1b1au), dg->compat);
         fprintf(flog, "file=(%d,%d,%d,%d), disk=(%d,%d,%d,%d), alias=(%d,%d,%d,%d)\n",
                       dg->afile.fileid,dg->afile.disk, dg->afile.aun, dg->afile.block,
                       dg->adisk.fileid,dg->adisk.disk, dg->adisk.aun, dg->adisk.block,
                       dg->alias.fileid,dg->alias.disk, dg->alias.aun, dg->alias.block);
     }
}

ub4 readDiskGroup(ASMDISKGROUP *dg, ub2 disk, ub4 au, ub2 blkn, ub1 buf[], ub4 buflen)
{
    ub2 pos=0;
    ub8 offset = 1;

    pos = dg->index[disk];
    if (pos >= ASM_MAX_DISKS) return 0;
    if (dg->disks[pos].dsknum != disk) return 0;
    
    offset = offset * au * dg->disks[pos].ausize;
    offset = offset + blkn * dg->disks[pos].blksize;
    return readFromDisk(&(dg->disks[pos]), offset, buf, buflen);
}

ub4 readDiskGroupAU(ASMDISKGROUP *dg, ub2 disk, ub4 au)
{
    if (dg->cache.au_disk == disk && dg->cache.au_aun == au) return dg->ausize;
    if (readDiskGroup(dg, disk, au, 0, dg->cache.aubuf, dg->ausize) == dg->ausize)
    {
        dg->cache.au_disk = disk;
        dg->cache.au_aun  = au;
        return dg->ausize;
    }
    return 0;
}

ub4 readDiskGroupEX(ASMDISKGROUP *dg, ub2 disk, ub4 au)
{
    if (dg->cache.ex_disk == disk && dg->cache.ex_aun == au) return dg->ausize;
    if (readDiskGroup(dg, disk, au, 0, dg->cache.exbuf, dg->ausize) == dg->ausize)
    {
        dg->cache.ex_disk = disk;
        dg->cache.ex_aun  = au;
        return dg->ausize;
    }
    return 0;
}

ub2 readDiskGroupBlock(ASMDISKGROUP *dg, ub2 disk, ub4 au, ub2 blkn)
{
    if (dg->cache.blk_disk == disk && dg->cache.blk_aun == au && dg->cache.blk_block == blkn) return dg->blksize;
    if (dg->cache.au_disk == disk && dg->cache.au_aun == au && (blkn+1) * dg->blksize < dg->ausize)
    {
        memcpy(dg->cache.blkbuf, dg->cache.aubuf + blkn * dg->blksize, dg->blksize);
        dg->cache.blk_disk = disk;
        dg->cache.blk_aun  = au;
        dg->cache.blk_block= blkn;
        return dg->blksize;        
    }
    if (dg->cache.ex_disk == disk && dg->cache.ex_aun == au && (blkn+1) * dg->blksize < dg->ausize)
    {
        memcpy(dg->cache.blkbuf, dg->cache.exbuf + blkn * dg->blksize, dg->blksize);
        dg->cache.blk_disk = disk;
        dg->cache.blk_aun  = au;
        dg->cache.blk_block= blkn;
        return dg->blksize;        
    }
    if (readDiskGroup(dg, disk, au, blkn, dg->cache.blkbuf, dg->blksize) == dg->blksize)
    {
        dg->cache.blk_disk = disk;
        dg->cache.blk_aun  = au;
        dg->cache.blk_block= blkn;
        return dg->blksize;
    }
    return 0;
}

void setDiskGroupCache(ASMDISKGROUP *dg)
{
    if (dg->ausize < 128 * 1028576)
    {
        if (dg->cache.aubuf != NULL) free(dg->cache.aubuf);
        dg->cache.aubuf = (ub1 *)malloc(dg->ausize);
        if (dg->cache.exbuf != NULL) free(dg->cache.exbuf);
        dg->cache.exbuf = (ub1 *)malloc(dg->ausize);
    }
    if (dg->blksize < 0xffff)
    {
        if (dg->cache.blkbuf != NULL) free(dg->cache.blkbuf);
        dg->cache.blkbuf = (ub1 *)malloc(dg->blksize);
    }
    dg->cache.blk_disk = 0xffff;
    dg->cache.blk_aun  = 0xffffff;
    dg->cache.blk_block= 0xffff;
    dg->cache.au_disk = 0xffff;
    dg->cache.au_aun  = 0xffffff;
    dg->cache.ex_disk = 0xffff;
    dg->cache.ex_aun  = 0xffffff;
}

/*
For file entry AU, fist block type should be ASM_BLOCK_LISTHEAD
Other blocks should be ASM_BLOCK_FILEDIR
*/

ub2  scoreASMFileEntryAU(ASMDISKGROUP *dg, ub2 disk, ub4 au)
{
    int i=0;
    ub2 score=0;
    ASMBH bh;

    if (readDiskGroupAU(dg, disk, au) == dg->ausize)
    {
        for(i=0;i * dg->blksize < dg->ausize; i++)
	{
            getASMBlockHeader(&bh, dg->cache.aubuf + i * dg->blksize);
            if (i>0)
	    {
                if (bh.type == ASM_BLOCK_FILEDIR && bh.blockblk == i) score ++;
            }
	    else
	    {
                if (bh.type == ASM_BLOCK_LISTHEAD && bh.blockblk == 0) score ++;
            }
	}
    }
    return score;
}

/*
    save the file entry information for file 1 and file 6
    #define ASM_FILE_FILEDIR  0x01
    #define ASM_FILE_DISKDIR  0x02
    #define ASM_FILE_ALIAS    0x06
*/
void getASMSystemFileEntry(ASMDISKGROUP *dg)
{
    int i=0;
    ASMBH bh;
    ASMFH fh;

    if (readDiskGroupAU(dg, dg->f1b1disk, dg->f1b1au) == dg->ausize)
    {
        for(i=0;i * dg->blksize < dg->ausize; i++)
	{
            getASMBlockHeader(&bh, dg->cache.aubuf + i * dg->blksize);
            if (bh.type == ASM_BLOCK_FILEDIR)
	    {
                getASMFileHeader(&fh, dg->cache.aubuf + i * dg->blksize);
                if (bh.blockblk == ASM_FILE_FILEDIR)
                {
                    dg->afile.fileid = ASM_FILE_FILEDIR;
                    dg->afile.disk   = dg->f1b1disk;
                    dg->afile.aun    = dg->f1b1au;
                    dg->afile.block  = i;
                }
                else if (bh.blockblk == ASM_FILE_DISKDIR)
                {
                    dg->adisk.fileid = ASM_FILE_DISKDIR;
                    dg->adisk.disk   = dg->f1b1disk;
                    dg->adisk.aun    = dg->f1b1au;
                    dg->adisk.block  = i;
                }
                else if (bh.blockblk == ASM_FILE_ALIAS)
                {
                    dg->alias.fileid = ASM_FILE_ALIAS;
                    dg->alias.disk   = dg->f1b1disk;
                    dg->alias.aun    = dg->f1b1au;
                    dg->alias.block  = i;
                }
            }
	}
    }
}

void listASMFileEntryAU(FILE *flog, ASMDISKGROUP *dg, ub2 disk, ub4 au)
{
    int blkid=0;
    ASMBH bh;
    ASMFH fh;
    ub8 allbytes=0;

    if (readDiskGroupAU(dg, disk, au) == dg->ausize)
    {
        for(blkid=0;blkid * dg->blksize < dg->ausize; blkid++)
	{
            getASMBlockHeader(&bh, dg->cache.aubuf + blkid * dg->blksize);
            if (bh.type == ASM_BLOCK_FILEDIR && bh.blockobj == 1)
	    {
                getASMFileHeader(&fh, dg->cache.aubuf + blkid * dg->blksize);
                if (fh.lobytes)
		{
                    allbytes = fh.hibytes;
                    allbytes = allbytes << 32;
                    allbytes = allbytes + fh.lobytes;
                    fprintf(flog,"fileid=%d, disk=%d, au=%d, block=%d, size=%lld, aus=%d, flags=%d, type=%d\n",
                            bh.blockblk, disk, au, blkid, allbytes, fh.xtntcnt, fh.flags, fh.filetype);
                }
            }
	}
    }
}

/*
Variable AU related
    1 - 20000 =  1 AU
20001 - 40000 =  8 AU
40001 -       = 64 AU
*/
ub4 getExtentAUCount(ASMEXTENTPOLICY *policy, ub4 extentid)
{
    if (extentid <= policy->level1au) 
        return policy->level1sz;
    else if (extentid <= policy->level2au) 
        return policy->level2sz;
    else if (extentid <= policy->level3au) 
        return policy->level3sz;
    else if (extentid <= policy->level4au) 
        return policy->level4sz;
    else
        return 1;
}

void setExtentAUCount(ASMEXTENTPOLICY *policy, ub1 level, ub4 auend, ub4 aucnt)
{
    if (aucnt == 0) return;

    if (level == 0x01)
    {
        policy->level1au = auend;
        policy->level1sz = aucnt;
    }
    else if (level == 0x02)
    {
        if (auend > policy->level1au)
        {
            policy->level2au = auend;
            policy->level2sz = aucnt;
        }
    }
    else if (level == 0x03)
    {
        if (auend > policy->level2au && auend > policy->level1au)
        {
            policy->level3au = auend;
            policy->level3sz = aucnt;
        }
    }
    else if (level == 0x04)
    {
        if (auend > policy->level3au && auend > policy->level2au && auend > policy->level1au)
        {
           policy->level4au = auend;
           policy->level4sz = aucnt;
        }
    }
}

void listASMFileExtent(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf)
{
    int entryi,extenti,blkid;
    ub4 allxtntcnt=0, dbxtntcnt = 0;
    ub8 allbytes=0;
    ASMBH      entrybh,extentbh;
    ASMFH      entryfh;
    ASMEXTH    extenth;
    ASMEXTENT  entryfet,extentfet;
    ub1        *entrybuf = NULL;

    entrybuf = (ub1 *)malloc(dg->blksize);
    if (entrybuf != NULL && readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         getASMBlockHeader(&entrybh, entrybuf);
         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == asmf->fileid)
         {
             getASMFileHeader(&entryfh, entrybuf);
             allbytes = entryfh.hibytes;
             allbytes = allbytes << 32;
             allbytes = allbytes + entryfh.lobytes;
             fprintf(flog,"fid=%d, disk=%d, au=%d, block=%d\n",
                     asmf->fileid, asmf->disk, asmf->aun, asmf->block);
             fprintf(flog,"siz=%lld, ecnt=%d, eeof=%d, flags=%d, type=%d, alias=%d,%d\n",
                     allbytes/1048576, entryfh.xtntcnt, entryfh.xtnteof, entryfh.flags,
                     entryfh.filetype, entryfh.alias0, entryfh.alias1);
             fprintf(flog,"     extid  disk       auid  cnt flg chk\n");
             fprintf(flog,"========== ===== ========== ==== === ===\n");
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                if (allxtntcnt < entryfh.xtntcnt)
                {
                    getASMFileExtent(&entryfet, dg->cache.blkbuf, dg->blksize, entryi);
                    fprintf(flog, "%10d %5d %10d %4d %3d %3d\n",
                            dbxtntcnt+1 , entryfet.disk, entryfet.au, 
                            getExtentAUCount(&(dg->policy),allxtntcnt + 1), entryfet.flags, entryfet.chk);
                }
                allxtntcnt ++;
		dbxtntcnt ++;
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
                   if (allxtntcnt >= entryfh.xtntcnt) break;
		   getASMFileExtent(&entryfet, entrybuf,dg->blksize, entryi);
                   readDiskGroupEX(dg, entryfet.disk, entryfet.au);
                   allxtntcnt ++;
                   for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                   {
                      if (allxtntcnt >= entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                      {
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == asmf->fileid)
                         {
                            getASMExtentHeader(&extenth, dg->cache.blkbuf);
                            if (dbxtntcnt < extenth.dxsn)
                            {
                                if (allxtntcnt < entryfh.xtntcnt)
                                {
                                   fprintf(flog, "ERROR: missing (%d) extents\n", extenth.dxsn - dbxtntcnt);
				   dbxtntcnt = extenth.dxsn;
                                   allxtntcnt = allxtntcnt + (extenth.dxsn - dbxtntcnt);
                                }
                            }
                            if (dbxtntcnt == extenth.dxsn)
                            {
                               for(extenti=0;extenti<extenth.xtntblk;extenti++)
	                       {
                                  if (allxtntcnt < entryfh.xtntcnt)
                                  {
                                     getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
                                     fprintf(flog, "%10d %5d %10d %4d %3d %3d\n",
                                             dbxtntcnt+1, extentfet.disk, extentfet.au,
                                             getExtentAUCount(&(dg->policy),dbxtntcnt+1),
                                             extentfet.flags, extentfet.chk);
                                     dbxtntcnt ++;
                                     allxtntcnt ++;
                                  }
                               }
                            }
                         }
                      }
                   }
             }
         }
         free(entrybuf);
    }
}

/*
List all files in the ASM
*/
void listASMFileEntry(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf)
{
    int entryi, extenti, blkid, fileid;
    ub4 allxtntcnt=0, dbxtntcnt = 0;
    ub8 allbytes=0;
    ASMBH      entrybh,extentbh,filebh;
    ASMFH      entryfh,filefh;
    ASMEXTH    extenth;
    ASMEXTENT  entryfet,extentfet;
    ub1        *entrybuf=NULL;

    if (asmf->fileid == 0xffffffff || asmf->fileid == 0) return;

    entrybuf = (ub1 *) malloc(dg->blksize);
    if (entrybuf != NULL && readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         getASMBlockHeader(&entrybh, entrybuf);

         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == asmf->fileid)
         {
             getASMFileHeader(&entryfh, entrybuf);
             fprintf(flog, "    file  disk       au blk     size flag type alias0,alias1              \n");
             fprintf(flog, "======== ===== ======== === ======== ==== ==== ===========================\n");
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                 getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                 /* read au into cache */
                 readDiskGroupAU(dg, entryfet.disk, entryfet.au);
                 for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                 {
                    if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                    {
                        getASMBlockHeader(&filebh, dg->cache.blkbuf);
                        if (filebh.type == ASM_BLOCK_FILEDIR && filebh.blockblk)
			{
                            getASMFileHeader(&filefh, dg->cache.blkbuf);
                            if (filefh.hibytes || filefh.lobytes)
                            {
                               allbytes = filefh.hibytes;
                               allbytes = allbytes << 32;
                               allbytes = allbytes + filefh.lobytes;
                               fprintf(flog,"%8d %5d %8d %3d %8lld %4d %4d %d,%d\n",
                                       filebh.blockblk, entryfet.disk, entryfet.au, blkid, allbytes/1048576,
                                       filefh.flags, filefh.filetype, filefh.alias0, filefh.alias1);
                            }
                        }
                    }
                 }
                 dbxtntcnt ++;
                 allxtntcnt ++;
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
		   getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                   /* read indirect au into ex cache */
                   readDiskGroupEX(dg, entryfet.disk, entryfet.au);
                   allxtntcnt ++;
                   for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                   {
                      if (allxtntcnt >= entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                      {
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == asmf->fileid)
                         {
                            getASMExtentHeader(&extenth, dg->cache.blkbuf);
                            for(extenti=0;extenti<extenth.xtntblk;extenti++)
	                    {
                                  getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
                                  readDiskGroupAU(dg, extentfet.disk, extentfet.au);
                                  for(fileid=0; fileid < dg->ausize/dg->blksize; fileid++)
                                  {
                                     if (readDiskGroupBlock(dg, extentfet.disk, extentfet.au, fileid) == dg->blksize)
                                     {
                                         getASMBlockHeader(&filebh, dg->cache.blkbuf);
                                         if (filebh.type == (ASM_BLOCK_FILEDIR & filebh.blockblk))
                 			 {
                                             getASMFileHeader(&filefh, dg->cache.blkbuf);
                                             if (filefh.hibytes || filefh.lobytes)
                                             {
                                                allbytes = filefh.hibytes;
                                                allbytes = allbytes << 32;
                                                allbytes = allbytes + filefh.lobytes;
                                                fprintf(flog,"%8d %5d %8d %3d %8lld %4d %4d %d,%d\n",
                                                        filebh.blockblk, extentfet.disk, extentfet.au, fileid, allbytes/1048576,
                                                        filefh.flags, filefh.filetype, filefh.alias0, filefh.alias1);
                                             }
                                         }
                                     }
                                 }
                                 dbxtntcnt ++;
                                 allxtntcnt ++;
                            }
                         }
                      }
                   }
             }
         }
         free(entrybuf);
    }
}

/*
get file entry information by fileid
read f1b1
*/

void getASMFileEntry(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 file_no)
{
    int entryi, extenti, blkid, fileid;
    ub4 allxtntcnt=0, dbxtntcnt = 0;
    ub8 allbytes=0;
    ASMBH      entrybh,extentbh,filebh;
    ASMFH      entryfh,filefh;
    ASMEXTH    extenth;
    ASMEXTENT  entryfet,extentfet;
    ub1        *entrybuf=NULL;

    if (readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
        getASMBlockHeader(&filebh, dg->cache.blkbuf);
        if (filebh.type == ASM_BLOCK_FILEDIR && filebh.blockblk == file_no && file_no == asmf->fileid) return;
    }

    entrybuf = (ub1 *) malloc(dg->blksize);
    if (entrybuf != NULL && readDiskGroupBlock(dg, dg->f1b1disk, dg->f1b1au, 1) == dg->blksize)
    {
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         getASMBlockHeader(&entrybh, entrybuf);

         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == 1)
         {
             getASMFileHeader(&entryfh, entrybuf);
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                 getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                 /* read au into cache */
                 readDiskGroupAU(dg, entryfet.disk, entryfet.au);
                 for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                 {
                    if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                    {
                        getASMBlockHeader(&filebh, dg->cache.blkbuf);
                        if (filebh.type == ASM_BLOCK_FILEDIR && filebh.blockblk)
			{
                            if (filebh.blockblk == file_no)
                            {
                                asmf->fileid = filebh.blockblk;
                                asmf->disk   = entryfet.disk;
                                asmf->aun    = entryfet.au;
                                asmf->block  = blkid;
				free(entrybuf);
				return;
                            }
                        }
                    }
                 }
                 dbxtntcnt ++;
                 allxtntcnt ++;
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
		   getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                   /* read indirect au into ex cache */
                   readDiskGroupEX(dg, entryfet.disk, entryfet.au);
                   allxtntcnt ++;
                   for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                   {
                      if (allxtntcnt >= entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                      {
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == 1)
                         {
                            getASMExtentHeader(&extenth, dg->cache.blkbuf);
                            for(extenti=0;extenti<extenth.xtntblk;extenti++)
	                    {
                                  getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
                                  readDiskGroupAU(dg, extentfet.disk, extentfet.au);
                                  for(fileid=0; fileid < dg->ausize/dg->blksize; fileid++)
                                  {
                                     if (readDiskGroupBlock(dg, extentfet.disk, extentfet.au, fileid) == dg->blksize)
                                     {
                                         getASMBlockHeader(&filebh, dg->cache.blkbuf);
                                         if (filebh.type == (ASM_BLOCK_FILEDIR & filebh.blockblk))
                 			 {
                                             if (filebh.blockblk == file_no)
                                             {
                                                 asmf->fileid = filebh.blockblk;
                                                 asmf->disk   = extentfet.disk;
                                                 asmf->aun    = extentfet.au;
                                                 asmf->block  = fileid;
                 				 free(entrybuf);
                 				 return;
                                             }
                                         }
                                     }
                                 }
                                 dbxtntcnt ++;
                                 allxtntcnt ++;
                            }
                         }
                      }
                   }
             }
         }
         free(entrybuf);
    }
    return;
}

void listASMFileExtent2(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf, ub4 extentid)
{
    int i;
    readFileExtentCache(dg, asmf, extentid);
    for(i=0;i<ASM_EXTENT_CACHE;i++)
    {
        if (asmf->extcache.id[i] != 0xffffffff)
        {
            fprintf(flog, "extentid=%d, disk=%d, au=%d\n", 
                    asmf->extcache.id[i], asmf->extcache.disk[i], asmf->extcache.au[i]);
        }
    }
}

/*
list all alias entry
1, aliasrow.fnum
2, aliasrow.finc
3, aliashead.parent_number
4, aliashead.fstblk_number
5, aliasrow.flags
6, aliasrow.refer.number
7, aliasrow.name
*/
void listASMAliasEntry(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf)
{
    int entryi,extenti, blkid, entryid, aliasid=0;
    ub4 allxtntcnt=0, dbxtntcnt = 0;
    ub8 allbytes=0;
    ASMBH      entrybh,extentbh,aliasbh;
    ASMFH      entryfh;
    ASMEXTH    extenth;
    ASMEXTENT  entryfet,extentfet;
    ASMAH      aliashead;
    ASMALIAS   aliasrow;
    ub1        *entrybuf = NULL;

    if (asmf->fileid == 0xffffffff || asmf->fileid == 0) return;

    entrybuf = (ub1 *) malloc (dg->blksize);
    if (entrybuf != NULL && readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         getASMBlockHeader(&entrybh, entrybuf);

         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == asmf->fileid)
         {
             getASMFileHeader(&entryfh, entrybuf);
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                 getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                 /* read the alias AU */
                 readDiskGroupAU(dg, entryfet.disk, entryfet.au);
                 fprintf(flog, "    file          inc parent fstblk flag  refer alias                     \n");
                 fprintf(flog, "======== ============ ====== ====== ==== ====== ==========================\n");
                 for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                 {
                    if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                    {
                        getASMBlockHeader(&aliasbh, dg->cache.blkbuf);
                        if (aliasbh.type == ASM_BLOCK_ALIASDIR && aliasbh.blockobj)
			{
                            /* loop here and print out the alias */
                            getASMAliasHeader(&aliashead, dg->cache.blkbuf);
                            for(aliasid=0;aliasid * sizeof(ASMALIAS)<dg->blksize; aliasid++)
                            {
                                getASMAliasRecord(&aliasrow, dg->cache.blkbuf, dg->blksize, aliasid);
                                if (aliasrow.name[0] && aliasrow.entry_hash )
                                {
                                    if (aliasrow.flags == 4 || aliasrow.flags == 18)
                                    {
                                        fprintf(flog, "%8d %12d %6d %6d %4d %6d %s\n", 
                                                aliasrow.fnum, aliasrow.finc,aliashead.parent_number, aliashead.fstblk_number,
                                                aliasrow.flags, aliasrow.refer_number, aliasrow.name); 
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                    }
                    dbxtntcnt ++;
                    allxtntcnt ++;
                }
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
		   getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                   /* read the indirect AU into cache */
                   readDiskGroupEX(dg, entryfet.disk, entryfet.au);
                   allxtntcnt ++;
                   for(blkid=0; blkid < dg->ausize/dg->blksize; blkid++)
                   {
                      if (allxtntcnt >= entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, blkid) == dg->blksize)
                      {
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == asmf->fileid)
                         {
                            getASMExtentHeader(&extenth, dg->cache.blkbuf);
                            for(extenti=0;extenti<extenth.xtntblk;extenti++)
	                    {
                                  getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
                                  /* read alias AU into cache */
                                  readDiskGroupAU(dg, extentfet.disk, extentfet.au);
                                  for(entryid=0; entryid < dg->ausize/dg->blksize; entryid++)
                                  {
                                     if (readDiskGroupBlock(dg, extentfet.disk, extentfet.au, entryid) == dg->blksize)
                                     {
                                         getASMBlockHeader(&aliasbh, dg->cache.blkbuf);
                                         if (aliasbh.type == (ASM_BLOCK_ALIASDIR & aliasbh.blockblk))
                 			 {
                                             /* loop here and print out the alias */
                                             getASMAliasHeader(&aliashead, dg->cache.blkbuf);
                                             for(aliasid=0;aliasid * sizeof(ASMALIAS)<dg->blksize; aliasid++)
                                             {
                                                 getASMAliasRecord(&aliasrow, dg->cache.blkbuf, dg->blksize, aliasid);
                                                 if (aliasrow.name[0] && aliasrow.entry_hash)
                                                 {
						     if (aliasrow.flags == 4 || aliasrow.flags == 18)
                                                     {
                                                         fprintf(flog, "%8d %12d %6d %6d %4d %6d %s\n", 
                                                                 aliasrow.fnum, aliasrow.finc,aliashead.parent_number, aliashead.fstblk_number,
                                                                 aliasrow.flags, aliasrow.refer_number, aliasrow.name); 
                                                     }
                                                 }
                                                 else
                                                 {
                                                     break;
                                                 }
                                             }
                                         }
                                     }
                                 }
                                 dbxtntcnt ++;
                                 allxtntcnt ++;
                            }
                         }
                      }
                   }
             }
         }
         free(entrybuf);
    }
}

/*
list ASM extent policy
*/
void listASMExtentPolicy(FILE *flog, ASMDISKGROUP *dg)
{
     fprintf(flog, "    au start       au end    aunum\n");
     fprintf(flog, "============ ============ ========\n");
     fprintf(flog, "%12u %12u %8u\n", 
             1, dg->policy.level1au, dg->policy.level1sz);
     if (dg->policy.level1au < 0xffffffff)
     {
          fprintf(flog, "%12u %12u %8u\n",
                 dg->policy.level1au + 1, dg->policy.level2au, dg->policy.level2sz);
          if (dg->policy.level2au < 0xffffffff)
          {
                 fprintf(flog, "%12u %12u %8u\n",
                         dg->policy.level2au + 1, dg->policy.level3au, dg->policy.level3sz);
                 if (dg->policy.level3au < 0xffffffff)
                 {
                         fprintf(flog, "%12u %12u %8u\n",
                                 dg->policy.level3au + 1, dg->policy.level4au, dg->policy.level4sz);
                 }
          }
     }
}


/*
copy file to os by ASM file describer
*/
void copyASMFileOut(FILE *fdat, ASMDISKGROUP *dg, ASMFILE *asmf)
{
    int extenti, entryi,exblkid;
    ub4 allxtntcnt=0, dbxtntcnt = 0,  varexti=0, varextcnt=0;
    ub8 allbytes=0;
    ASMBH      entrybh, extentbh;
    ASMFH      entryfh;
    ASMEXTH    extentfeh;
    ASMEXTENT  entryfet,extentfet;
    ub1        *entrybuf=NULL;

    entrybuf = (ub1 *) malloc (dg->blksize);
    if (entrybuf != NULL  && readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
         /* save file entry to local buffer */
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         /* process the file entry block */
         getASMBlockHeader(&entrybh, entrybuf);
         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == asmf->fileid)
         {
             getASMFileHeader(&entryfh, entrybuf);

             /* adjust the extent count in file header, for data files, it may contains one empty AU */
             allbytes = entryfh.hibytes;
             allbytes = allbytes << 32;
             allbytes = allbytes + entryfh.lobytes;
             /*
             xtntcnt  = allbytes/dg->ausize;
             if (allbytes % dg->ausize) xtntcnt ++;
             entryfh.xtntcnt = MIN(entryfh.xtntcnt, xtntcnt);
             */
             dbxtntcnt = 0;
             allxtntcnt = 0;
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                if (allxtntcnt < entryfh.xtntcnt)
                {
                    getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                    /* clear the io buf */
                    dg->cache.ex_disk = 0xffff;
                    dg->cache.ex_aun  = 0xffffffff;
		    if (readDiskGroupEX(dg, entryfet.disk, entryfet.au) == dg->ausize)
                    {
                        /* read success,write the readed content */
                        fwrite(dg->cache.exbuf, dg->ausize, 1, fdat);
                    }
                    else
                    { 
                        /*+ read failure, write empty au */
                        dg->cache.ex_disk = 0xffff;
                        dg->cache.ex_aun  = 0xffffffff;
                        memset(dg->cache.exbuf, 0, dg->ausize);
                        fwrite(dg->cache.exbuf, dg->ausize, 1, fdat);
                    }
                }
                dbxtntcnt  ++;
		allxtntcnt ++;
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
                   if (allxtntcnt >= entryfh.xtntcnt) break;
                   /* get the indirect AU location */
		   getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                   /* read the indirect AU into normal buf */
                   readDiskGroupAU(dg, entryfet.disk, entryfet.au);
                   /* for each indirect AU, process each block */
                   allxtntcnt ++;
                   for(exblkid=0; exblkid < dg->ausize/dg->blksize; exblkid++)
                   {
                      if (allxtntcnt > entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, exblkid) == dg->blksize)
                      {
                         /* each block of indirect au, contains the file au information */
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == asmf->fileid)
                         {
                            getASMExtentHeader(&extentfeh, dg->cache.blkbuf);
                            /* extentfeh.dxsn means the start extent id */
                            if (dbxtntcnt < extentfeh.dxsn)
                            {
                                /* missing some extent here */
                                for(extenti=0;extenti<extentfeh.dxsn - dbxtntcnt;extenti++)
                                {
                                    if (dbxtntcnt + extenti + 1>= entryfh.xtntcnt) break;
                                    varextcnt = getExtentAUCount(&(dg->policy),dbxtntcnt + extenti + 1);
                                    for(varexti = 0; varexti < varextcnt; varexti ++)
                                    {
                                        dg->cache.ex_disk = 0xffff;
                                        dg->cache.ex_aun  = 0xffffffff;
                                        memset(dg->cache.exbuf, 0, dg->ausize);
                                        fwrite(dg->cache.exbuf, dg->ausize, 1, fdat);
                                    }
                                }
                                allxtntcnt= allxtntcnt + extenti;
                                dbxtntcnt = dbxtntcnt + extenti;
                            }
                            if (dbxtntcnt == extentfeh.dxsn)
                            {
                               for(extenti=0;extenti<extentfeh.xtntblk;extenti++)
	                       {
                                  if (allxtntcnt < entryfh.xtntcnt)
                                  {
                                         getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
                                         dg->cache.ex_disk = 0xffff;
                                         dg->cache.ex_aun  = 0xffffffff;
                                         varextcnt = getExtentAUCount(&(dg->policy),dbxtntcnt+1);
                                         for (varexti=0;varexti<varextcnt;varexti++)
                                         {
                                             if (readDiskGroupEX(dg, extentfet.disk, extentfet.au + varexti) == dg->ausize)
                                             {
                                                 fwrite(dg->cache.exbuf, dg->ausize, 1, fdat);
                                             }
                                             else
                                             {
                                                 dg->cache.ex_disk = 0xffff;
                                                 dg->cache.ex_aun  = 0xffffffff;
                                                 memset(dg->cache.exbuf, 0, dg->ausize);
                                                 fwrite(dg->cache.exbuf, dg->ausize, 1, fdat);
                                             }
                                         }
                                  }
                                  else
                                  {
                                         break;
                                  }
                                  dbxtntcnt ++;
                                  allxtntcnt ++;
                               }
                            }
                         }
                      }
                }
             }
         }
         free(entrybuf);
    }
}

void readFileExtentCache(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 extentid)
{
    int extenti, entryi,exblkid;
    ub4 allxtntcnt=0, dbxtntcnt = 0,  varexti=0, varextcnt=0;
    ub8 allbytes=0;
    ASMBH      entrybh, extentbh;
    ASMFH      entryfh;
    ASMEXTH    extentfeh;
    ASMEXTENT  entryfet,extentfet;
    ub1        *entrybuf=NULL;

    if (extentid && asmf->extcache.id[extentid % ASM_EXTENT_CACHE] == extentid) return;
    /* invalid the extent cache */
    for(extenti = 0; extenti < ASM_EXTENT_CACHE; extenti ++)
    {
        asmf->extcache.id[extenti] = 0xffffffff;
        asmf->extcache.disk[extenti] = 0xffff;
        asmf->extcache.au[extenti] = 0xffffffff;
    }
    entrybuf = (ub1 *) malloc (dg->blksize);
    if (entrybuf != NULL  && readDiskGroupBlock(dg, asmf->disk, asmf->aun, asmf->block) == dg->blksize)
    {
         /* save file entry to local buffer */
         memcpy(entrybuf, dg->cache.blkbuf, dg->blksize);
         /* process the file entry block */
         getASMBlockHeader(&entrybh, entrybuf);
         if (entrybh.type == ASM_BLOCK_FILEDIR && entrybh.blockblk == asmf->fileid)
         {
             getASMFileHeader(&entryfh, entrybuf);

             /* adjust the extent count in file header, for data files, it may contains one empty AU */
             allbytes = entryfh.hibytes;
             allbytes = allbytes << 32;
             allbytes = allbytes + entryfh.lobytes;
             /*
             xtntcnt  = allbytes/dg->ausize;
             if (allbytes % dg->ausize) xtntcnt ++;
             entryfh.xtntcnt = MIN(entryfh.xtntcnt, xtntcnt);
             */
             dbxtntcnt = 0;
             allxtntcnt = 0;
             for(entryi=0;entryi<MIN(entryfh.xtntblk, entryfh.xbreak);entryi++)
	     {
                if (allxtntcnt < entryfh.xtntcnt)
                {
                    getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                    if (dbxtntcnt + 1 >= extentid && dbxtntcnt+1 < extentid + ASM_EXTENT_CACHE)
                    {
			/*+ entryfet.disk, entryfet.au */
                        asmf->extcache.id[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = dbxtntcnt + 1;
                        asmf->extcache.disk[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = entryfet.disk;
                        asmf->extcache.au[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = entryfet.au;
                    }                    
                }
                dbxtntcnt  ++;
		allxtntcnt ++;
             }
             for(entryi=entryfh.xbreak;entryi<entryfh.xtntblk;entryi++)
	     {
                   if (allxtntcnt  >= entryfh.xtntcnt) break;
                   if (dbxtntcnt+1 >= extentid + ASM_EXTENT_CACHE) break;
                   /* get the indirect AU location */
		   getASMFileExtent(&entryfet, entrybuf, dg->blksize, entryi);
                   /* read the indirect AU into normal buf */
                   readDiskGroupAU(dg, entryfet.disk, entryfet.au);
                   /* for each indirect AU, process each block */
                   allxtntcnt ++;
                   for(exblkid=0; exblkid < dg->ausize/dg->blksize; exblkid++)
                   {
                      if (allxtntcnt > entryfh.xtntcnt) break;
                      if (readDiskGroupBlock(dg, entryfet.disk, entryfet.au, exblkid) == dg->blksize)
                      {
                         /* each block of indirect au, contains the file au information */
                         getASMBlockHeader(&extentbh, dg->cache.blkbuf);
                         if (extentbh.type == ASM_BLOCK_INDIRECT && extentbh.blockobj == asmf->fileid)
                         {
                            getASMExtentHeader(&extentfeh, dg->cache.blkbuf);
                            /* extentfeh.dxsn means the start extent id */
                            if (dbxtntcnt < extentfeh.dxsn)
                            {
                                /* missing some extent here */
                                for(extenti=0;extenti<extentfeh.dxsn - dbxtntcnt;extenti++)
                                {
                                    if (dbxtntcnt + extenti + 1 >= entryfh.xtntcnt) break;
                                    if (dbxtntcnt + extenti + 1 > extentid + ASM_EXTENT_CACHE) break;
   		                    /*+ entryfet.disk, entryfet.au */
                                    if (dbxtntcnt + extenti + 1 >= extentid && dbxtntcnt + extenti + 1 < extentid + ASM_EXTENT_CACHE)
                                    {
                                        asmf->extcache.id[(dbxtntcnt + extenti + 1) % ASM_EXTENT_CACHE] = dbxtntcnt + extenti + 1;
                                        asmf->extcache.disk[(dbxtntcnt + extenti + 1) % ASM_EXTENT_CACHE] = 0xffff;
                                        asmf->extcache.au[(dbxtntcnt + extenti + 1) % ASM_EXTENT_CACHE] = 0xffffffff;
                                    }
                                }
                                allxtntcnt= allxtntcnt + extenti;
                                dbxtntcnt = dbxtntcnt + extenti;
                            }
                            if (dbxtntcnt == extentfeh.dxsn)
                            {
                               for(extenti=0;extenti<extentfeh.xtntblk;extenti++)
	                       {
                                  if (allxtntcnt < entryfh.xtntcnt && dbxtntcnt+1 < extentid + ASM_EXTENT_CACHE)
                                  {
                                         getASMFileExtent(&extentfet, dg->cache.blkbuf, dg->blksize, extenti);
					 if (dbxtntcnt+1 >= extentid && dbxtntcnt+1 < extentid + ASM_EXTENT_CACHE)
                                         {
                                            asmf->extcache.id[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = dbxtntcnt + 1;
                                            asmf->extcache.disk[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = extentfet.disk;
                                            asmf->extcache.au[(dbxtntcnt + 1) % ASM_EXTENT_CACHE] = extentfet.au;
                                         }
                                  }
                                  else
                                  {
                                         break;
                                  }
                                  dbxtntcnt ++;
                                  allxtntcnt ++;
                               }
                            }
                         }
                      }
                }
             }
         }
         free(entrybuf);
    }
}

/*
calc the auid according to the file offset
*/
void  getASMFileOffsetAU(ASMDISKGROUP *dg, ASMAUOFFSET *auoffs, ub8 offset)
{
     ub4 au_cnt=0;
     ub8 u8_tmp=0;

     auoffs->au     = 0;
     auoffs->sz     = 0;
     auoffs->offset = 0;

     au_cnt = offset/dg->ausize;
     if (au_cnt * dg->policy.level1sz < dg->policy.level1au)
     {
         /* in the fisrt level range */
         auoffs->au     = au_cnt/dg->policy.level1sz;
         auoffs->offset = au_cnt;
         auoffs->offset = auoffs->offset *  dg->ausize;
         auoffs->sz     = dg->policy.level1sz;
     }
     else
     {
         /* in the second level range */
         auoffs->au     = dg->policy.level1au;
         auoffs->offset = dg->policy.level1au;
         auoffs->offset = auoffs->offset * dg->policy.level1sz * dg->ausize;
         
         /* remain au count */
         au_cnt  = au_cnt - dg->policy.level1au * dg->policy.level1sz;
         if (au_cnt * dg->policy.level2sz < ((dg->policy.level2au - dg->policy.level1au) * dg->policy.level2sz))
         {
             auoffs->au    = auoffs->au    +  au_cnt/dg->policy.level2sz;
             u8_tmp = (au_cnt/dg->policy.level2sz);
             u8_tmp = u8_tmp * dg->ausize * dg->policy.level2sz;
             auoffs->offset= auoffs->offset + u8_tmp;
             auoffs->sz     = dg->policy.level2sz;
         }
         else
         {
             /* in the second level range */
             auoffs->au     = auoffs->au    + (dg->policy.level2au - dg->policy.level1au);
             u8_tmp         = (dg->policy.level2au - dg->policy.level1au);
             u8_tmp         = u8_tmp * dg->ausize * dg->policy.level2sz;
             auoffs->offset = auoffs->offset + u8_tmp;

             au_cnt  = au_cnt - (dg->policy.level2au - dg->policy.level1au) * dg->policy.level2sz;
             if (au_cnt * dg->policy.level3sz < ((dg->policy.level3au - dg->policy.level2au) * dg->policy.level3sz))
             {
                 auoffs->au    = auoffs->au    +  au_cnt/dg->policy.level3sz;
                 u8_tmp = (au_cnt/dg->policy.level3sz);
                 u8_tmp = u8_tmp * dg->ausize * dg->policy.level3sz;
                 auoffs->offset= auoffs->offset + u8_tmp;
                 auoffs->sz     = dg->policy.level3sz;
             }
             else
             {
                 /* in the second level range */
                 auoffs->au     = auoffs->au    + (dg->policy.level3au - dg->policy.level2au);
                 u8_tmp         = (dg->policy.level3au - dg->policy.level2au);
                 u8_tmp         = u8_tmp * dg->ausize * dg->policy.level3sz;
                 auoffs->offset = auoffs->offset + u8_tmp;

                 au_cnt  = au_cnt - (dg->policy.level3au - dg->policy.level2au) * dg->policy.level3sz;
                 auoffs->au     = auoffs->au     + au_cnt/dg->policy.level4sz;
                 u8_tmp         = (dg->policy.level4au - dg->policy.level3au);
                 u8_tmp         = u8_tmp * dg->ausize * dg->policy.level4sz;
                 auoffs->offset = auoffs->offset + u8_tmp;
                 auoffs->sz     = dg->policy.level4sz;
             }
         }
     }
}

/* 
read data block from ASM according to Oracle block id
*/
ub4  readASMFileData(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 oblockid, ub2 oblksize, ub1 *buf, ub4 buflen)
{
     ub4 au_offs = 0, au_pos=0, piece1=0, piece2=0;
     ub8 u8_offset=0;
     ASMAUOFFSET auoffs;

     u8_offset = oblockid;
     u8_offset = u8_offset * oblksize;     
     getASMFileOffsetAU(dg, &auoffs, u8_offset);

     if (u8_offset >= auoffs.offset && buflen < dg->ausize)
     {
         u8_offset = u8_offset - auoffs.offset;
         readFileExtentCache(dg, asmf, auoffs.au + 1);
         if (asmf->extcache.id[(auoffs.au + 1) % ASM_EXTENT_CACHE] == auoffs.au + 1)
         {
             au_offs = u8_offset / dg->ausize;
             au_pos  = u8_offset % dg->ausize;
             if (au_offs < auoffs.sz)
             {
                 if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                    asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE] + au_offs) == dg->ausize)
                 {
                     if (au_pos + buflen <= dg->ausize)
                     {
                         memcpy(buf, dg->cache.exbuf + au_pos, buflen);
                         return buflen;
                     }
		     else
                     {
                         /* get the first piece */
                         piece1 = dg->ausize - au_pos;
                         memcpy(buf, dg->cache.exbuf + au_pos, piece1);

                         /* get second piece */                         
                         piece2 = au_pos + buflen - dg->ausize;
                         if (au_pos + buflen <= auoffs.sz * dg->ausize)
                         {
                              /* in the same extent */
                              if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                                 asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE] + au_offs + 1) == dg->ausize)
                              {
                                   memcpy(buf+piece1, dg->cache.exbuf, piece2);
                                   return buflen;
                              }
                         }
                         else
                         {
                              auoffs.au ++;
                              readFileExtentCache(dg, asmf, auoffs.au + 1);
                              if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                                 asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE]) == dg->ausize)
                              {
                                   memcpy(buf+piece1, dg->cache.exbuf, piece2);
                                   return buflen;
                              }
                         }
                     }
                 }
             }
         }
     }
     return 0;
}

/*
read Oracle block from ASM according to Oracle block id and block count
read one by one, try to get more blocks
*/
ub4  readASMOracleBlock(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 oblockid,  ub2 oblksize, ub1 *buf, ub4 blkcnt)
{
     ub4 au_offs = 0, au_pos=0, piece1=0, piece2=0, buflen=0;
     ub8 u8_offset=0;
     ASMAUOFFSET auoffs;

     u8_offset = oblockid;
     u8_offset = u8_offset * oblksize;     
     getASMFileOffsetAU(dg, &auoffs, u8_offset);

     if (u8_offset >= auoffs.offset && oblksize * blkcnt < dg->ausize)
     {
         u8_offset = u8_offset - auoffs.offset;
         readFileExtentCache(dg, asmf, auoffs.au + 1);
         if (asmf->extcache.id[(auoffs.au + 1) % ASM_EXTENT_CACHE] == auoffs.au + 1)
         {
             au_offs = u8_offset / dg->ausize;
             au_pos  = u8_offset % dg->ausize;
             if (au_offs < auoffs.sz)
             {
                 if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                    asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE] + au_offs) == dg->ausize)
                 {
                     if (au_pos + oblksize * blkcnt <= dg->ausize)
                     {
                         memcpy(buf, dg->cache.exbuf + au_pos, oblksize * blkcnt);
                         buflen = oblksize * blkcnt;
                     }
		     else
                     {
                         /* get the first piece */
                         piece1 = dg->ausize - au_pos;
                         memcpy(buf, dg->cache.exbuf + au_pos, piece1);
			 buflen = (piece1/oblksize) * oblksize;

                         /* get second piece */                         
                         piece2 = au_pos + buflen - dg->ausize;
                         if (au_pos + buflen <= auoffs.sz * dg->ausize)
                         {
                              /* in the same extent */
                              if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                                 asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE] + au_offs + 1) == dg->ausize)
                              {
                                   memcpy(buf+piece1, dg->cache.exbuf, piece2);
                                   buflen = ((piece1 + piece2)/oblksize) * oblksize;
                              }
                         }
                         else
                         {
                              auoffs.au ++;
                              readFileExtentCache(dg, asmf, auoffs.au + 1);
                              if(readDiskGroupEX(dg, asmf->extcache.disk[(auoffs.au + 1) % ASM_EXTENT_CACHE], 
                                                 asmf->extcache.au[(auoffs.au + 1) % ASM_EXTENT_CACHE]) == dg->ausize)
                              {
                                   memcpy(buf+piece1, dg->cache.exbuf, piece2);
                                   buflen = ((piece1 + piece2)/oblksize) * oblksize;
                              }
                         }
                     }
                 }
             }
         }
     }
     return buflen;
}

/*
dump oracle block
*/
void dumpASMFileBlock(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf, ub4 blockid)
{
     ub1 *buf=NULL;

     buf = (ub1 *) malloc (dg->oblksize);
     if (buf != NULL && readASMFileData(dg, asmf, blockid, dg->oblksize, buf, dg->oblksize) == dg->oblksize)
     {
         printBlock(flog, buf, dg->oblksize);
         free(buf);
     }
}
