## Compiler Output:

****************

```sh
gcc -c -g -Wall testing.c -o testing.o
gcc -c -g -Wall lodepng.c -o lodepng.o
gcc -c -g -Wall imageData.c -o imageData.o
gcc -c -g -Wall mp8.c -o mp8.o
gcc -c -g -Wall mp8recurse.c -o mp8recurse.o
gcc -g -Wall testing.o lodepng.o imageData.o mp8.o mp8recurse.o -o grader

```



Grade:

******

Late submission: 

basicFlood works correctly: 10/10

basicRecurse works correctly: 20/20

colorsWithinDistSq works correctly: 5/5

greyFlood works correctly: 10/10

greyRecurse works correctly: 0/15

limitedFlood works correctly: 10/10

limitedRecurse works correctly: 15/15

Functionality score: 70/85


late punishment: 0

Functionality Final score:: 70
