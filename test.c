#include <stdio.h>



int swap_nums(int *a, int *b){
    int temp;

     temp = *b;
     *b = *a;
     *a = temp;

}


int main(){
    int a = 15;
    int b =24;

     printf("%d swapped with %d becomes: \n", a,b);
    swap_nums(&a , &b);

    
    printf("%d swapped with %d becomes: ", a, b);


};
