
#include <jni.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>

static lua_State *L = NULL;

// Luaの初期化
jint Java_com_example_nativelua_MainActivity_initLua(JNIEnv* env, jobject thiz) {
  if (L == NULL) {
    // Luaの初期化
    L = luaL_newstate();
    // Luaライブラリの初期化
    luaL_openlibs(L);
  }
  return 0;
}

// Luaの終了
jint Java_com_example_nativelua_MainActivity_closeLua(JNIEnv* env, jobject thiz) {
  if (L != NULL) {
    // Lua終了
    lua_close(L);
    L = NULL;
  }
  return 0;
}

// 整数値(Integer)を取得する
jint Java_com_example_nativelua_MainActivity_getInteger(JNIEnv* env,
                                                        jobject thiz,
                                                        jstring valuename) {
  // Cの文字列に変換する
  const jchar* c_vname = (*env)->GetStringUTFChars(env, valuename, JNI_FALSE);
  // グローバル変数の値をスタックに積む
  lua_getglobal(L, c_vname);
  // スタックから値を読む
  int value = lua_tointeger(L, -1);
  // スタックから値を取り除く
  lua_pop(L, 1);
  // 文字列領域を解放する
  (*env)->ReleaseStringChars(env, valuename, c_vname);

  return value;
}

// スクリプトを実行する
void Java_com_example_nativelua_MainActivity_runScript(JNIEnv* env,
                                                       jobject thiz,
                                                       jobject script) {

  // Cの文字列に変換する
  const char* c_script = (*env)->GetStringUTFChars(env, script, JNI_FALSE);
  // スクリプトを実行する
  luaL_dostring(L, c_script);
  // 文字列領域を解放する
  (*env)->ReleaseStringChars(env, script, c_script);
}

