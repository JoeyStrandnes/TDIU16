//LAB 1
#include <stdio.h>


int main(int argc, char** argv){

  int counter = 0;
/*
  int temp[] = {69,2,3,4};
  int* ptr = temp;

  printf("%li \n", sizeof(*(ptr)));
*/
  if (argc > 20) {
    printf("%s\n", "Matat in för många");
  }

  else{
    //printf("%li\n", sizeof(*argv[1]));

    for (int i = 0; i < argc; i++) {
      char* ptr = argv[i];
      printf("%s",ptr);
      printf(" %p",ptr);
      printf(" %p",argv[i]);
      printf(" %li", sizeof(*(ptr)));
      printf(" %li \n", sizeof(*(argv[i])));
      for(unsigned int k = 0; k < sizeof(*ptr); k++){
        printf("%d ", k);
        printf("%c", (argv[i][k]));

        counter++;
      }

      printf("    %d\n", counter);
      counter = 0;
    }
  }


  return 0;

}