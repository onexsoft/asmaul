/*
*
*                                 Apache License
*                           Version 2.0, January 2004
*                        http://www.apache.org/licenses/
*/

#ifndef _ASMKENEL_H_
#define _ASMKENEL_H_

#include <stdio.h>

/*
   Type defination
*/
typedef          char      sb1;
typedef unsigned char      ub1;
typedef unsigned short     ub2;
typedef          short     sb2;
typedef unsigned int       ub4;
typedef          int       sb4;
typedef unsigned long long ub8;
typedef          long long sb8;

/* Trial Version Max Disk Count */
#define ASM_MAX_DISKS     2048
#define ASM_EXTENT_CACHE   256

#ifdef TRIAL_VERSION
#undef  ASM_MAX_DISKS
#define ASM_MAX_DISKS       16
#endif

#ifdef TRIAL_BINARY
#undef  ASM_MAX_DISKS
#define ASM_MAX_DISKS       16
#endif

#define ASM_BIG_EDIAN     0x00
#define ASM_LITTLE_EDIAN  0x01

/* ASM Metadata Files */
#define ASM_FILE_METADATA 0x00
#define ASM_FILE_FILEDIR  0x01
#define ASM_FILE_DISKDIR  0x02
#define ASM_FILE_ALIAS    0x06

/* ASM Metadata Block Types */
#define ASM_BLOCK_DISKHEAD 0x01
#define ASM_BLOCK_FILEDIR  0x04
#define ASM_BLOCK_LISTHEAD 0x05
#define ASM_BLOCK_DISKDIR  0x06
#define ASM_BLOCK_ALIASDIR 0x0b
#define ASM_BLOCK_INDIRECT 0x0c
#define ASM_BLOCK_VOLDIR   0x16

#define LB2UB2(b1,b2) (((b2) << 8) + (b1))
#define LB2UB4(b1,b2,b3,b4) \
	(((b4) << 24)+((b3) << 16)+((b2) << 8) + (b1))

#define BB2UB2(b2,b1) (((b2) << 8) + (b1))
#define BB2UB4(b4,b3,b2,b1) \
	(((b4) << 24)+((b3) << 16)+((b2) << 8) + (b1))


#define  MIN(a,b) ((a) > (b) ? (b) : (a))
#define  MAX(a,b) ((a) < (b) ? (b) : (a))

/*
Get following output with "kfed read xxx aun=0 blkn=0"

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

typedef struct _ASMBH
{
  ub1 edian;
  ub1 hard;
  ub1 type;
  ub1 datfmt;
  ub4 blockblk;
  ub4 blockobj;
  ub4 check;
  ub4 fcnbase;
  ub4 fcnwrap;
  ub4 spare1;
  ub4 spare2;
} ASMBH;

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

typedef struct _ASMMETA
{
  ub1 provstr[8];
  ub4 compat;
  ub2 dsknum;
  ub1 grptyp;
  ub1 hdrsts;
  ub1 dskname[32];
  ub1 grpname[32];
  ub1 fgname [32];
  ub1 capname[32];
  ub2 secsize;
  ub2 blksize;
  ub4 ausize;
  ub4 mfact;
  ub4 dsksize;
  ub4 pmcnt;
  ub4 fstlocn;
  ub4 altlocn;
  ub4 f1b1locn;
  ub4 dbcompat;
} ASMMETA;

/*
kffixb.dxsn:                         60 ; 0x000: 0x0000003c
kffixb.xtntblk:                      69 ; 0x004: 0x0045
kffixb.dXrs:                         17 ; 0x006: SCHE=0x1 NUMB=0x1
kffixb.ub1spare:                      0 ; 0x007: 0x00
kffixb.ub4spare:                      0 ; 0x008: 0x00000000
*/

typedef struct _ASMEXTH
{
   ub4 dxsn;
   ub2 xtntblk;
   ub1 dxrs;
   ub1 ub1spare;
   ub4 ub4spare;
} ASMEXTH;

/*
kfffde[0].xptr.au:                    2 ; 0x4a0: 0x00000002
kfffde[0].xptr.disk:                  0 ; 0x4a4: 0x0000
kfffde[0].xptr.flags:                 0 ; 0x4a6: L=0 E=0 D=0 S=0
kfffde[0].xptr.chk:                  40 ; 0x4a7: 0x28
*/

typedef struct _ASMEXTENT
{
   ub4 au;
   ub2 disk;
   ub1 flags;
   ub1 chk;
} ASMEXTENT;

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

typedef struct _ASMFH
{
     ub4 hibytes;
     ub4 lobytes;
     ub4 xtntcnt;
     ub4 xtnteof;
     ub4 blksize;
     ub1 flags;
     ub1 filetype;
     ub1 dxrs;
     ub1 ixrs;
     ub4 dxsiz0;
     ub4 dxsiz1;
     ub4 dxsiz2;
     ub4 ixsiz0;
     ub4 ixsiz1;
     ub4 ixsiz2;
     ub2 xtntblk;
     ub2 xbreak;
     ub1 prizn;
     ub1 seczn;
     ub4 alias0;
     ub4 alias1;
     ub1 strpwdth;
     ub1 strpsz;
     ub2 usmsz;
} ASMFH;

/*
ASM Alias Header 
block type = ASM_BLOCK_ALIAS
a, buffer header
b, alias  header
c, alias
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

typedef struct _ASMAH
{
    ub4 bnode_incarn;
    ub4 frlist_number;
    ub4 frlist_incarn;
    ub4 overfl_number;
    ub4 overfl_incarn;
    ub4 parent_number;
    ub4 parent_incarn;
    ub4 fstblk_number;
    ub4 fstblk_incarn;
} ASMAH;

/*
ASM Alias
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

typedef struct _ASMALIAS
{
  ub4 entry_incarn;
  ub4 entry_hash;
  ub4 refer_number;
  ub4 refer_incarn;
  ub1 name[48];
  ub4 fnum;
  ub4 finc;
  ub1 flags;
  ub1 ub1spare;
  ub2 ub2spare;
} ASMALIAS;

/*
ASM Disk Structure
The most important attribute is dsknum.
*/

typedef struct _ASMDISK
{
    ub2 dsknum;
    ub2       blksize;
    ub4       ausize;    
    ub4 dsksize;
    ub1 dskname[32];
    ub1 grpname[32];
    ub1 path[256];
    FILE *fp;
} ASMDISK;

/*
Disk Group Read Global Cache
*/
typedef struct _ASMCACHE
{
  ub2 blk_disk;
  ub4 blk_aun;
  ub2 blk_block;
  ub1 *blkbuf;
  ub2 au_disk;
  ub4 au_aun;
  ub1 *aubuf;
  ub2 ex_disk;
  ub4 ex_aun;
  ub1 *exbuf;
} ASMCACHE;

/*
file entry 
*/

typedef struct _ASMEXTENTCACHE
{
  ub4 id[ASM_EXTENT_CACHE];
  ub2 disk[ASM_EXTENT_CACHE];
  ub4 au[ASM_EXTENT_CACHE];
} ASMEXTENTCACHE;

typedef struct _ASMFILE
{
    ub4 fileid;
    ub2 disk;
    ub4 aun;
    ub2 block;
    ASMEXTENTCACHE extcache;
} ASMFILE;

/*
define the ASM variable extent policy
*/

typedef struct _ASMEXTENTPOLICY
{
   /* Level 1, if auid <= level1au, then return level1sz */
   ub4 level1au;
   ub4 level1sz;

   /* level 2, if auid <= level2au, then return level2sz */
   ub4 level2au;
   ub4 level2sz;

   /* level 3, if auid <= level2au, then return level2sz */
   ub4 level3au;
   ub4 level3sz;

   /* level 4, if auid <= level2au, then return level2sz */
   ub4 level4au;
   ub4 level4sz;
}  ASMEXTENTPOLICY;

/*
ASM Disk Group Structure
Contains an index (position in the disks array) for disks by disk number.
And find out the f1b1 location (include disk and au, file directory location)
First two AU is reserved for file 0, if cannot found it, we need search it.
*/

typedef struct _ASMDISKGROUP
{
    ub4       compat;
    ub2       oblksize;
    ub2       blksize;
    ub4       ausize;         
    ub2       dskcnt;
    ub2       f1b1disk;
    ub4       f1b1au;
    ub2       index[65536];
    ASMDISK   disks[ASM_MAX_DISKS];
    ASMCACHE  cache;
    ASMFILE   afile;
    ASMFILE   alias;
    ASMFILE   adisk;
    ASMEXTENTPOLICY policy;
} ASMDISKGROUP;

/*
get the AU offset, and the file size
*/
typedef struct _ASMAUOFFSET
{
    ub4 au;
    ub2 sz;
    ub8 offset;
} ASMAUOFFSET;

void  printDate(FILE *fp);
void  printBlock(FILE *fp,unsigned char *buf,size_t len);

void getASMBlockHeader(ASMBH *bh, ub1 buf[]);
void getASMBlockMetaData(ASMMETA *bh, ub1 buf[]);
void getASMFileHeader(ASMFH *bh, ub1 buf[]);
void getASMFileExtent(ASMEXTENT *bh, ub1 buf[], ub2 buflen, ub2 xid);
void getASMExtentHeader(ASMEXTH *bh, ub1 buf[]);
void getASMAliasHeader(ASMAH *bh, ub1 buf[]);
void getASMAliasRecord(ASMALIAS *bh, ub1 buf[], ub2 buflen, ub2 xid);

void initASMDisk(ASMDISK *disk);
void initASMDiskGroup(ASMDISKGROUP *dg);

void openDiskGroup(FILE *flog, ASMDISKGROUP *dg, ub1 *fname);
void openDiskGroupDisk(FILE *flog, ASMDISKGROUP *dg, ub2 disk, ub1 *fname);
void closeDiskGroup(ASMDISKGROUP *dg);
ub4 readFromDisk(ASMDISK *dsk, ub8 offset, ub1 buf[], ub4 buflen);
ub4 readDiskGroup(ASMDISKGROUP *dg, ub2 disk, ub4 au, ub2 blkn, ub1 buf[], ub4 buflen);
ub4 readDiskGroupAU(ASMDISKGROUP *dg, ub2 disk, ub4 au);
ub4 readDiskGroupEX(ASMDISKGROUP *dg, ub2 disk, ub4 au);
ub2 readDiskGroupBlock(ASMDISKGROUP *dg, ub2 disk, ub4 au, ub2 blkn);
void printDiskGroup(FILE *flog, ASMDISKGROUP *dg);
void setDiskGroupCache(ASMDISKGROUP *dg);

ub2  scoreASMFileEntryAU(ASMDISKGROUP *dg, ub2 disk, ub4 au);
void getASMSystemFileEntry(ASMDISKGROUP *dg);
void getASMFileEntry(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 fileid);

/*
Variable AU related
    1 - 20000 =  1 AU
20001 - 40000 =  8 AU
40001 -       = 64 AU
*/
ub4 getExtentAUCount(ASMEXTENTPOLICY *policy, ub4 extentid);
void setExtentAUCount(ASMEXTENTPOLICY *policy, ub1 level, ub4 auend, ub4 aucnt);

/*
list feature
*/
void listASMFileEntry(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf);
void listASMAliasEntry(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf);
void listASMFileEntryAU(FILE *flog, ASMDISKGROUP *dg, ub2 disk, ub4 au);
void listASMFileExtent(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf);
void listASMFileExtent2(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf, ub4 extentid);
void listASMExtentPolicy(FILE *flog, ASMDISKGROUP *dg);
void dumpASMFileBlock(FILE *flog, ASMDISKGROUP *dg, ASMFILE *asmf, ub4 blockid);

/*
copy feature
*/
void copyASMFileOut(FILE *fdat, ASMDISKGROUP *dg, ASMFILE *asmf);
void readFileExtentCache(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 extentid);
void getASMFileOffsetAU(ASMDISKGROUP *dg, ASMAUOFFSET *auoffs, ub8 offset);
ub4  readASMFileData(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 oblockid, ub2 oblksize, ub1 *buf, ub4 buflen);
ub4  readASMOracleBlock(ASMDISKGROUP *dg, ASMFILE *asmf, ub4 oblockid, ub2 oblksize, ub1 *buf, ub4 blkcnt);

#endif


