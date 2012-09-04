/////begin neon_samplecode_01
#include <jni.h>

#define SIZE 30000

void multi(int* __restrict a, int* __restrict b, int* __restrict c, int size){
  int i;
  for (i = 0; i < size; i++){
    *c = *a * *b;
    a++;
    b++;
    c++;
  }
}
void Java_com_example_neon_MainActivity_multi(JNIEnv* env, jobject thiz){

  int lst1[SIZE],lst2[SIZE],lst3[SIZE];

  int i;
  for (i = 0; i < SIZE; i++){
    lst1[i] = 0x10+i;
    lst2[i] = 0x20+i*2;
  }

  multi(lst1, lst2, lst3, SIZE);
}
/////end
