# asmaul

A command line utility to interactive with Oracle ASM storage, code extracted from AUL utility (a utility for Oracle data recovery to read rows from damaged Oracle database files directly, visit http://www.mydul.net for more details).

ASM> help;

  SET        -- set options (BLOCK_SIZE, AU_SIZE, F1B1_DISK, F1B1_AU)

  OPEN       -- open data file

  ADD        -- add disk with customized disk num (disk, path)

  LIST       -- list opened disks information (to)

  LSAU       -- list file of given AU (disk, au, count)

  FILE       -- list all file entries in ASM (file, [disk, au, block]).

  ALIAS      -- list all alias entries in ASM (file, [disk, au, block]).

  EXTENT     -- list or adjust variable extent (level, au, count).
  
  FMAP       -- list file extent (file, [disk, au, block], to)
  
  DUMP       -- dump file extent (file, [disk, au, block], offset, to)
  
  COPY       -- copy file to os  (file, [disk, au, block], to)
  
  QUIT/EXIT  -- exit the program.
  
#build & compile
gcc -O2 -o aulasm.bin aulasm2.c asmkernel.c

#contact
For any questions, please submit a issue.
