Compiler Output:
****************

```sh
gcc -Wall -g  -c -o mp11.o mp11.c
gcc -g -o c220 ece220_lex.yy.o ece220_parse.tab.o ece220_symtab.o ece220_label.o mp11base.o mp11.o
```



Grade:

******

Returns a constant: 4/4

Negation works correctly: 2/2

Adding works correctly: 2/2

Subtracting works correctly: 2/2

Adding and subtracting work recursively: 1/1

Calling a function with an integer as an argument works correctly: 7/7

Calling a function with a string as an argument works correctly: 7/7

Calling multiple functions works correctly: 3/3

Creating and assigning variables works correctly: 4/4

Calling functions with variables as arguments works correctly: 4/4

Multiplication works correctly: 1/1

Division works correctly: 1/1

Modulus works correctly: 1/1

Arrays work correctly: 4/4

Incrementing and decrementing work correctly: 2/2

Simple for loops work correctly: 4/4

More complicated for loops work correctly: 3/3

Long for loops work correctly: 2/2

Getting the address of a variable works correctly: 6/6

If statements work correctly: 4/4

Else statements work correctly: 3/3

Not expressions work correctly: 3/3

Or expressions work correctly: 4/4

Or expressions end if the left side is a 1: 2/2

And expressions work correctly: 4/4

And expressions end if the left side is a 0: 2/2

Multiple logical expressions work together correctly: 3/3

Functionality score: 85/85


late punishment: 0

Functionality Final score:: 85
