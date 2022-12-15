## Compiler Output:  

****************  

```sh
gcc -c -Wall -g -o mp6.o mp6.c
gcc -c -Wall -g -o main.o main.c
gcc mp6.o main.o -o mp6 -lm -lpng -g
```  

  

## Result:  

******  

convert_RGB_to_HSL passed: 3/3  

convert_HSL_to_RGB passed: 3/3  

compute_sobel_kernels passed: 3/3  

 equalize_intensities passed: 3/3  

Functionality score: 70/70  

late punishment: 0

Functionality Final score:: 70
