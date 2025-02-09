#include <stdio.h>  // Be és kimenetekhez határoz meg változókat,makrókat és funckiókat.
#include <stdlib.h> // Általános függvény végrehajtásokhoz.

//argc = hány db karakter
//argv = argumentumok listája
int main(int argc, char **argv)
//char** means an array of character arrays = array of strings
{
    int n = 25;
    int *k = &n; // pointer assignment

    printf("n = %i\n", n);  // Prints 25
    printf("k = %p\n", (void*)k);  // Prints the address of n
    printf("*k = %i\n", *k);  // Prints the value stored at address k (which is 25)

    return 0;
}

// %p is the proper way to print a pointer in C
// *k (dereferencing) gives the value stored at n, which is 25