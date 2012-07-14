#include <assert.h>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "jnihelper.h"

#include <android/log.h>
#define TAG "NativeMedia"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)


// ネイティブメディアのヘッダファイル
#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>

#include <android/native_window_jni.h>

// engineインターフェイス
static XAObjectItf engineObject = NULL;
static XAEngineItf engineEngine = NULL;

//　output mixインターフェイス
static XAObjectItf outputMixObject = NULL;

// ストリーミングメディアプレイヤーのインターフェイス
static XAObjectItf playerObj = NULL;
static XAPlayItf playerPlayItf = NULL;
static XAAndroidBufferQueueItf playerBQItf = NULL;
static XAStreamInformationItf playerStreamInfoItf = NULL;
static XAVolumeItf playerVolItf = NULL;
// プレイヤーのためのビデオシンク
static ANativeWindow* theNativeWindow;

// メディアプレイヤーを生成するのに必要なインターフェイスの数
#define NB_MAXAL_INTERFACES 3 // XAAndroidBufferQueueItf, XAStreamInformationItf and XAPlayItf
// バッファーキューの数（数値は適当）
#define NB_BUFFERS 8
// MPEG-2トランスポートパケットサイズ
#define MPEG2_TS_PACKET_SIZE 188
// バッファあたりのMEPG-2トランスポートパケット数（数値は適当）
#define PACKETS_PER_BUFFER 10
// メモリーキャッシュするためのメモリーサイズ
#define BUFFER_SIZE (PACKETS_PER_BUFFER*MPEG2_TS_PACKET_SIZE)
// 再生するデータを格納するキャッシュエリア
// このメモリーはバッファーキューコールバックによって再利用される
static char dataCache[BUFFER_SIZE * NB_BUFFERS];

// 再生するファイルのハンドル
static FILE *file;
// ファイルの最後(EndOfFile)まで届いたか？
static jboolean reachedEof = JNI_FALSE;
// ストリーム最後のバッファーコンテキストIDを定義
static const int kEosBufferCntxt = 1980; // 数値はマジックナンバー
// mutexでコールバックスレッドとアプリケーションスレッドの同期を取る
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// condは再生の中断を通知するためのシグナルとなる
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// 再生中断しているか？
static jboolean discontinuity = JNI_FALSE;

static jboolean enqueueInitialBuffers(jboolean discontinuity);



/////begin ch_multi_samplecode_6
//　MEPG-2 TSパケットからメディアプレイヤーにデータを供給するためのコールバック
static XAresult AndroidBufferQueueCallback(XAAndroidBufferQueueItf caller,
                                           void *pCallbackContext,
                                           void *pBufferContext,
                                           void *pBufferData,
                                           XAuint32 dataSize,
                                           XAuint32 dataUsed,
                                           const XAAndroidBufferItem *pItems,
                                           XAuint32 itemsLength ) {
  pthread_mutex_lock(&mutex);

  // 再生中断の要求があったか？
  if (discontinuity) {                        /////-----(1)ここから
    if (!reachedEof) {
       // バッファーキューをクリアーする
      (*playerBQItf)->Clear(playerBQItf);
      // ファイルポインタを最初に戻す
      rewind(file);
      // 最初のバッファを中断の指示を含めてキューに貯めこむ
      (void) enqueueInitialBuffers(JNI_TRUE);
    }

    discontinuity = JNI_FALSE;
    pthread_cond_signal(&cond);

    goto exit;                                       /////-----(1)ここまで
  }

  if ((pBufferData == NULL) && (pBufferContext != NULL)) { /////-----(2)ここから
    const int processedCommand = *(int *) pBufferContext;
    // EOS(End of stream)か？
    if (kEosBufferCntxt == processedCommand) {
      LOGV("EOS was processed\n");
      // 読み込み処理終了
      goto exit;
    }
  } /////-----(2)ここまで

  // 一度EOF(EndOfFile)にヒットしたら、これ以上読み込まない
  if (reachedEof) {
    goto exit;
  }

  size_t nbRead;

  // freadは複数のスレッドから呼ばれるが、同時に行なってはならない
  size_t bytesRead;
  bytesRead = fread(pBufferData, 1, BUFFER_SIZE, file); /////-----(3)
  if (bytesRead > 0) { /////-----(4) ここから
    if ((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) { 
      LOGV("Dropping last packet because it is not whole");
    }
    size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
    size_t bufferSize = packetsRead * MPEG2_TS_PACKET_SIZE;
    LOGV("read packetsRead=%d, bufferSize=%d",packetsRead,bufferSize);
    (*caller)->Enqueue(caller,
                       NULL /*pBufferContext*/,
                       pBufferData /*pData*/,
                       bufferSize /*dataLength*/,
                       NULL /*pMsg*/,
                       0 /*msgLength*/);
  } else {
     // EOF(EndOfFile)か I/Oエラーが発生したらEOS(EndOfStream)を発行する
    XAAndroidBufferItem msgEos[1];
    msgEos[0].itemKey = XA_ANDROID_ITEMKEY_EOS;
    msgEos[0].itemSize = 0;
    // EOSメッセージは、パラメータを持たない。
    // メッセージのサイズは、keyとitemSizesの合計（どちらもXAuint32型）
    (*caller)->Enqueue(caller,
                       (void *) &kEosBufferCntxt /*pBufferContext*/,
                       NULL /*pData*/, 
                       0 /*dataLength*/, 
                       msgEos /*pMsg*/,
                       sizeof(XAuint32) * 2 /*msgLength*/);
    reachedEof = JNI_TRUE;
  } /////-----(4) ここから

  exit: pthread_mutex_unlock(&mutex);
  return XA_RESULT_SUCCESS;
}
/////end

// ストリーミング情報が新しくなる、もしくは変化した場合は、このコールバックが呼び出される
static void StreamChangeCallback(XAStreamInformationItf caller,
                                 XAuint32 eventId, XAuint32 streamIndex,
                                 void * pEventData, void * pContext) {
  LOGV("StreamChangeCallback called for stream %u", streamIndex);

  switch (eventId) {
  case XA_STREAMCBEVENT_PROPERTYCHANGE: {
    XAuint32 domain;
    (*caller)->QueryStreamType(caller, streamIndex, &domain);
    switch (domain) {
    case XA_DOMAINTYPE_VIDEO: {
      // ビデオ情報取得
      XAVideoStreamInformation videoInfo;
      (*caller)->QueryStreamInformation(caller, streamIndex, &videoInfo);
      LOGV(
          "Found video size %u x %u, codec ID=%u, frameRate=%u, bitRate=%u, duration=%u ms", videoInfo.width, videoInfo.height, videoInfo.codecId, videoInfo.frameRate, videoInfo.bitRate, videoInfo.duration);
    }
      break;
    default:
      fprintf(stderr, "Unexpected domain %u\n", domain);
      break;
    }
  }
    break;
  default:
    fprintf(stderr, "Unexpected stream event ID %u\n", eventId);
    break;
  }
}


// 初期バッファをキューに貯めこむ
static jboolean enqueueInitialBuffers(jboolean discontinuity) {

  /* キャッシュを満たす
   * 全体のパケット（MPEG2_TS_PACKET_SIZEの整数倍）を読み込む。
   * freadは、バイト数ではなく要素の数をかえす。要素の数はパケットサイズの倍数であるかチェックする必要がある。
   */
  size_t bytesRead;
  bytesRead = fread(dataCache, 1, BUFFER_SIZE * NB_BUFFERS, file);
  if (bytesRead <= 0) {
    // 早まったEOF(EndOfFile)か、I/Oエラーである
    return JNI_FALSE;
  }
  if ((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) {
    LOGV("Dropping last packet because it is not whole");
  }
  size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
  LOGV("Initially queueing %u packets", packetsRead);

  // 再生前に、キャッシュにコンテントをキューに貯めこむ
  size_t i;
  for (i = 0; i < NB_BUFFERS && packetsRead > 0; i++) {
    // このバッファのサイズを計算する
    size_t packetsThisBuffer = packetsRead;
    if (packetsThisBuffer > PACKETS_PER_BUFFER) {
      packetsThisBuffer = PACKETS_PER_BUFFER;
    }
    size_t bufferSize = packetsThisBuffer * MPEG2_TS_PACKET_SIZE;
    if (discontinuity) {
      // 再生中断のシグナル
      XAAndroidBufferItem items[1];
      items[0].itemKey = XA_ANDROID_ITEMKEY_DISCONTINUITY;
      items[0].itemSize = 0;
      // DISCONTINUITYメッセージはパラメータを持たない。
      // メッセージのサイズは、keyとitemSizesの合計（どちらもXAuint32型）
      (*playerBQItf)->Enqueue(playerBQItf, 
                              NULL /*pBufferContext*/,
                              dataCache + i * BUFFER_SIZE, 
                              bufferSize,
                              items /*pMsg*/,
                              sizeof(XAuint32) * 2 /*msgLength*/);
      discontinuity = JNI_FALSE;
    } else {
      (*playerBQItf)->Enqueue(playerBQItf, 
                              NULL /*pBufferContext*/,
                              dataCache + i * BUFFER_SIZE, 
                              bufferSize,
                              NULL, 
                              0);
    }
    packetsRead -= packetsThisBuffer;
  }

  return JNI_TRUE;
}

//  ストリーミングメディアプレイヤーを巻き戻す
void rewindStreamingMediaPlayer(JNIEnv *env, jclass clazz) {
  XAresult res;

  // ストリーミングメディアプレイヤーが生成済であるかチェックする
  if (NULL != playerBQItf && NULL != file) {
    // 現在のキューが空になるのを待つ
    pthread_mutex_lock(&mutex);
    discontinuity = JNI_TRUE;

    // バッファーキューコールバックを監視するために再生中断要求を待つ
    while (discontinuity && !reachedEof) {
      pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
  }
}

/////begin ch_multi_samplecode_5
// ストリーミングメディアプレイヤーを生成する
jboolean createStreamingMediaPlayer(JNIEnv* env, jclass clazz, jstring filename) {
  XAresult res;

  // Javaの文字列を変換する
  const char *utf8 = (*env)->GetStringUTFChars(env, filename, NULL);

  // 再生するファイルを開く
  file = fopen(utf8, "rb");
  if (file == NULL) {
    return JNI_FALSE;
  }

  // データソースを指定する
  XADataLocator_AndroidBufferQueue loc_abq = {     /////-----(1)ここから
      XA_DATALOCATOR_ANDROIDBUFFERQUEUE, NB_BUFFERS };
  XADataFormat_MIME format_mime = {
      XA_DATAFORMAT_MIME,
      XA_ANDROID_MIME_MP2TS,
      XA_CONTAINERTYPE_MPEG_TS };
  XADataSource dataSrc = { &loc_abq, &format_mime };

  // オーディオシンクを構築する
  XADataLocator_OutputMix loc_outmix = {
      XA_DATALOCATOR_OUTPUTMIX,outputMixObject };
  XADataSink audioSnk = { &loc_outmix, NULL };

  // イメージビデオシンクを構築する
  // ビデオシンクは、SurfaceかSurfaceTextureから生成されたANativeWindowであること
  XADataLocator_NativeDisplay loc_nd = {
    XA_DATALOCATOR_NATIVEDISPLAY,
    (void*) theNativeWindow,
    NULL // 必ずNULL
  };
  XADataSink imageVideoSink = { &loc_nd, NULL };

  // インターフェイスの使い方を宣言する
  XAboolean required[NB_MAXAL_INTERFACES] = {
    XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE};
  XAInterfaceID iidArray[NB_MAXAL_INTERFACES] = {
    XA_IID_PLAY, XA_IID_ANDROIDBUFFERQUEUESOURCE, XA_IID_STREAMINFORMATION };

  // メディアプレイヤーを生成する
  (*engineEngine)->CreateMediaPlayer(
      engineEngine, &playerObj, &dataSrc, NULL, &audioSnk, &imageVideoSink,
      NULL, NULL, 
      NB_MAXAL_INTERFACES /*XAuint32 numInterfaces*/,
      iidArray /*const XAInterfaceID *pInterfaceIds*/,
      required /*const XAboolean *pInterfaceRequired*/);   

  // Java,UTF-8文字列を解放する
  (*env)->ReleaseStringUTFChars(env, filename, utf8);

  // メディアプレイヤーを実体化する
  (*playerObj)->Realize(playerObj, XA_BOOLEAN_FALSE); /////-----(1)ここまで

  // playerインターフェイスを取得する                  /////-----(2)ここから
  (*playerObj)->GetInterface(playerObj, XA_IID_PLAY, &playerPlayItf);

  // ストリーム情報インターフェイスを取得する（ビデオサイズ取得のため）
  (*playerObj)->GetInterface(playerObj, XA_IID_STREAMINFORMATION,
                                   &playerStreamInfoItf);

  // ボリュームインターフェイスを取得する
  (*playerObj)->GetInterface(playerObj, XA_IID_VOLUME, &playerVolItf);

  // Androidバッファキューインターフェイスを取得する
  (*playerObj)->GetInterface(playerObj, XA_IID_ANDROIDBUFFERQUEUESOURCE,
                                   &playerBQItf);

  // 発生して欲しいイベントセットする
  (*playerBQItf)->SetCallbackEventsMask(
      playerBQItf, XA_ANDROIDBUFFERQUEUEEVENT_PROCESSED);

  //　OpenMAX ALが再生するためのデータを読み出すためにコールバックを登録する
  (*playerBQItf)->RegisterCallback(playerBQItf,
                                         AndroidBufferQueueCallback, NULL);

  // コールバックの登録をする（再生時間などの動画情報を取得するため）
  (*playerStreamInfoItf)->RegisterStreamChangeCallback( /////-----(2)ここまで
      playerStreamInfoItf, StreamChangeCallback, NULL);

  // 初期バッファをキューに貯めこむ
  if (!enqueueInitialBuffers(JNI_FALSE)) { /////-----(3)
    return JNI_FALSE;
  }

  // プレイヤーの準備をする /////-----(4)ここから
  (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PAUSED);

  // 音量をセットする
  (*playerVolItf)->SetVolumeLevel(playerVolItf, 0);

  // 再生開始
  (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PLAYING);  /////-----(4)ここまで

  return JNI_TRUE;
}
/////end

// ストリーミングメディアプレイヤーのプレイ状態をセットする
void setPlayingStreamingMediaPlayer(JNIEnv* env, jclass clazz,
                                    jboolean isPlaying) {
  XAresult res;
  // ストリーミングメディアプレイヤーが生成されているかチェック
  if (NULL != playerPlayItf) {
    // プレイ状態をセットする
    res = (*playerPlayItf)->SetPlayState(
      playerPlayItf, isPlaying ? XA_PLAYSTATE_PLAYING : XA_PLAYSTATE_PAUSED);
  }
}

// ネイティブメディアシステムを終了する
void shutdown(JNIEnv* env, jclass clazz) {

  // streaming media playerオブジェクトの破棄と、関連する変数を無効化する
  if (playerObj != NULL) {
    (*playerObj)->Destroy(playerObj);
    playerObj = NULL;
    playerPlayItf = NULL;
    playerBQItf = NULL;
    playerStreamInfoItf = NULL;
    playerVolItf = NULL;
  }

  // output mixオブジェクトの破棄と、関連する変数を無効化する
  if (outputMixObject != NULL) {
    (*outputMixObject)->Destroy(outputMixObject);
    outputMixObject = NULL;
  }

  // engineオブジェクトの破棄と、関連する変数を無効化する
  if (engineObject != NULL) {
    (*engineObject)->Destroy(engineObject);
    engineObject = NULL;
    engineEngine = NULL;
  }

  // ファイルを閉じる
  if (file != NULL) {
    fclose(file);
    file = NULL;
  }

  // NativeWindowを解放する
  if (theNativeWindow != NULL) {
    ANativeWindow_release(theNativeWindow);
    theNativeWindow = NULL;
  }
}

/////begin ch_multi_samplecode_4
// サーフェイスをセットする
void setSurface(JNIEnv *env, jclass clazz, jobject surface) {
  // JavaのサーフェイスからNativeWindowを取得する
  theNativeWindow = ANativeWindow_fromSurface(env, surface); 
}
/////end

/////begin ch_multi_samplecode_3
// engineとoutput mixを生成する /////-----(1)ここから
void createEngine(JNIEnv* env, jclass clazz) {
  // engineを生成する
  xaCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
  // engineを実体化する
  (*engineObject)->Realize(engineObject, XA_BOOLEAN_FALSE);
  // engineインターフェイスやそれに関連するインターフェイスを取得する
  (*engineObject)->GetInterface(engineObject, XA_IID_ENGINE,
                                      &engineEngine);

  // outout mixを生成する
  (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0,
                                         NULL, NULL);
  // output mixを実体化する
  (*outputMixObject)->Realize(outputMixObject, XA_BOOLEAN_FALSE);
} /////-----(1)ここまで
/////end

static JNINativeMethod sMethods[] = {
{ "createEngine", "()V",                       
  (void*) createEngine },
{ "setSurface",   "(Landroid/view/Surface;)V", 
  (void*) setSurface },
{ "createStreamingMediaPlayer", "(Ljava/lang/String;)Z", 
  (void*) createStreamingMediaPlayer },
{ "rewindStreamingMediaPlayer", "()V",    
  (void*) rewindStreamingMediaPlayer },
{ "shutdown", "()V", 
  (void*) shutdown },
{ "setPlayingStreamingMediaPlayer","(Z)V", 
  (void*) setPlayingStreamingMediaPlayer },
};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env = NULL;
  jint result = -1;

  if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
    return result;
  }
  jniRegisterNativeMethods(env, "com/example/nativemediaplayer/MainActivity",
                           sMethods, NELEM(sMethods));
  return JNI_VERSION_1_6;
}
/////end
