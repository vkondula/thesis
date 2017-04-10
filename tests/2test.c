#include <stdio.h>

int a;

int mul(int a, int b){
	/* the worst mul function ever*/
	if (b) {
		return a + mul(a, b-1); 
	}
	return 0;
}

int main() {
   /* my first program in C */
   printf("Hello, World! \n");
   a = mul(3,4);
   if (a>3){
       printf("so unexpected!");
   }
   int b = mul(a, 2);
   return b;
}
