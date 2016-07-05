# asmaul
Read out files directly from Oracle ASM Disks or Image files.

A command line utility to interactive with Oracle ASM storage

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
  
