# asmaul

A command line utility to interactive with Oracle ASM storage, code extracted from AUL utility (a utility for Oracle data recovery to read rows from damaged Oracle database files directly, visit http://www.mydul.net for more details).

- Examples: http://www.mydul.net/asmexample.html
- Commands: http://www.mydul.net/aulasmcmd.html

# To do
A full disk scan logic should be added for damaged Oracle ASM group to reconstruct the Oracle data files.

# build & compile
Simply run:
  make

or
  gcc -O2 -DSIMPLE_FPOS_T -o aulasm.bin aulasm2.c asmkernel.c

If it shows up some errors about like 'incompatible types in assignment', try this:
  gcc -O2 -o aulasm.bin aulasm2.c asmkernel.c

# contact
For any questions, please submit a issue.
