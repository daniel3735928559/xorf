## Old Project

This project was found at an archeological digsite covering a little-explored corner of my hard drive.  It is well past its expiration date and in particular may no longer represent anyone's opinion 
about best-practices for either coding or functionality.  It is here in case the idea or code may have some value to someone, but the activation energy required for restarting actual development on this 
project is currently quite high.

## README

```
20120915_xorf_0.2: 

COMPILE: 

gcc -o xorf xorf.c

RUN: 

./xorf target file1 file2 ...

This will xor the contents of all the given files with the contents of target and send the result to stdout.  

Any files smaller than target will be effectively repeated until it is the same size as target, so xoring

aaaaaaaaaaa

with

bbcc

will be effectively xoring 

aaaaaaaaaaa

with

bbccbbccbbc

This has now been tested on more arbitrary data, and resulted in at least A^B^A == B

ISSUES: 

- Checks for end of file buffer on each byte, rather than just setting the correct intervals and running 
  without checking until the check is known to be necessary.  So it should work, and the code is simpler, 
  but there are around twice as many operations per byte as should be necessary

```
