#include <jni.h>

#define SIZE 100

void Java_com_example_neon_MainActivity_mult(JNIEnv* env, jobject thiz){

  int lst1[SIZE],lst2[SIZE],lst3[SIZE];
  int i;
  for (i = 0; i < SIZE; i++){
    lst1[i] = 10+i;
    lst2[i] = 20+i*2;
  }

  multi_mat(lst1, lst2, lst3);
}

void multi_mat(int* __restrict a, int* __restrict b, int* __restrict c){
  int i;
  for (i = 0; i < SIZE; i++){
    *c = *a * *b;
    a++;
    b++;
    c++;
  }
}
