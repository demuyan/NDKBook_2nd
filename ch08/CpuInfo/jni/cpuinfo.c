#include <jni.h>
#include <cpu-features.h>

/////begin ch08_samplecode_1
// 利用しているCPUアーキテクチャを判別する
AndroidCpuFamily getCpuArch(char* cpuinfo){

  AndroidCpuFamily cpuFamily = android_getCpuFamily();

  switch(cpuFamily){
  case ANDROID_CPU_FAMILY_ARM:     // CPUアーキテクチャがARM
    strcat(cpuinfo, "ARM ");
    break;
  case ANDROID_CPU_FAMILY_MIPS:    // CPUアーキテクチャがMIPS
    strcat(cpuinfo, "mips ");
    break;
  case ANDROID_CPU_FAMILY_X86:     // CPUアーキテクチャがx86
    strcat(cpuinfo, "x86 ");
    break;
  case ANDROID_CPU_FAMILY_UNKNOWN: // CPUアーキテクチャが不明
    strcat(cpuinfo, "UNKNOWN");
    break;
  }
  return cpuFamily;
}
/////end
/////begin ch08_samplecode_2
// 利用しているARMアーキテクチャの機能詳細を取得
void getArmInfo(char* cpuinfo){

  uint64_t features = android_getCpuFeatures();

  // ARMv7をサポート?
  if (features & ANDROID_CPU_ARM_FEATURE_ARMv7)
    strcat(cpuinfo, "ARMv7 ");

  // VFPv3をサポート?
  if (features & ANDROID_CPU_ARM_FEATURE_VFPv3)
    strcat(cpuinfo, "VFPv3 ");

  // NEONをサポート?
  if (features & ANDROID_CPU_ARM_FEATURE_NEON)
    strcat(cpuinfo, "NEON ");

  // LDREX/STREX命令をサポート?(ARMv6より)
  if (features & ANDROID_CPU_ARM_FEATURE_LDREX_STREX)
    strcat(cpuinfo, "LDREX_STREX ");
}
/////end
/////begin ch08_samplecode_3
// 利用しているx86アーキテクチャの機能詳細を取得
void getX86Info(char* cpuinfo){

  uint64_t features = android_getCpuFeatures();

  // SSE3をサポート?
  if (features & ANDROID_CPU_X86_FEATURE_SSSE3)
    strcat(cpuinfo, "SSSE3 ");

  // POPCNT命令をサポート?
  if (features & ANDROID_CPU_X86_FEATURE_POPCNT)
    strcat(cpuinfo, "POPCNT ");

  // MOVBE命令をサポート?
  if (features & ANDROID_CPU_X86_FEATURE_MOVBE)
    strcat(cpuinfo, "MOVBE ");
}
/////end
// CPU情報を取得する
jobject Java_com_example_cpuinfo_MainActivity_getCpuInfo(JNIEnv* env, jobject thiz) {

  char cpuinfo[0x100] = {0};

  AndroidCpuFamily cpuFamily = getCpuArch(cpuinfo);

  // CPUの詳細な情報を取得する
  switch(cpuFamily){
  case ANDROID_CPU_FAMILY_ARM:
    getArmInfo(cpuinfo);
    break;
  case ANDROID_CPU_FAMILY_X86:
    getX86Info(cpuinfo);
    break;
  }
  jobject jstr = (*env)->NewStringUTF(env, cpuinfo);
  return jstr;
}

