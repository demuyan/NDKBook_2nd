#include <jni.h>
#include <stdio.h>
#include <string.h>

void Java_com_example_clashapp_MainActivity_crashProcess(JNIEnv* env, jobject thiz) {

	char *buf = NULL;
	strcpy(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");  // ここでクラッシュする
}
