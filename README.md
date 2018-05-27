# 2D-Nim Board Equivalency #

This was an interesting project I found in the Berkeley 2001 Programming contest,
where, given two lists of x,y coordinates representing where the pieces lie on two boards,
the program must determine if the two boards are equivalent, meaning that the same winning strategy applies for both boards, i.e., the clusters of pieces on each board are geometrically the same, but may be rotated, reflected, moved, or any permutations of these translations.

## Compilation
to compile just run
```bash
cc -Wall -pedantic src/*.c
```

