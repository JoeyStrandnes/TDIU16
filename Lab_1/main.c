//LAB 1
#include <stdio.h>


int main(int argc, char** argv){


  int total = 0;
  int maxWord = 30;
  int spacing = 0;

  for (int i = 0; i < argc; i++)
  {
    int iterator = 0;
    while(argv[i][iterator] != '\0' && iterator < 20)
    {
      printf("%c", argv[i][iterator]);
      iterator++;
    }

    total += iterator;

    if (spacing < iterator)
    {
      spacing = maxWord-iterator;
    }

    printf("%*.d\n", maxWord-iterator,iterator);

  }

  printf("Total length %*.d\n", (spacing-6),total);
  printf("Average length %*.2f\n", (spacing-8), (total/(float)argc));

  return 0;

}
