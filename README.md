# 「AndroidNDK ネイティブプログラミング 第２版」サンプルコード集

「AndroidNDKネイティブプログラミング 第２版」に掲載したサンプルコード、その検証用に作成されたコードです。


## 開発環境
Eclipse, ADTr20, Android NDKr8bを利用しています。環境構築は本書第１章に書かれています。

## ファイルについて

Chapter別に分けてあります。

### ch01/
- AddApp/   加算演算(Javaのみ)
- AppAppJni/ 加算演算(Java,C/C++)

### ch02/
- JniApp/   JNI関数のサンプルコード集
 
### ch03/
- GLNativeActivity　NativeActivity＆OpenGL｜ES1.1でスプライト表示

### ch04/
- CubeDroid11/   立方体表示（NativeActivity, OpenGL|ES1.1）  
- cubedroid20/   立方体表示（NativeActivity, OpenGL|ES2.0）  
- NativeBitmap/ NativeBitmapサンプル(セピア調変換)  

### ch06/
- NativeMediaPlayer ネイティブメディアプレイヤーを使ったシンプルな動画プレイヤー

### ch07/
- InputCheck/    タッチパネル、センサーからのデータ取得、表示
- AccessAssets/  Assetsマネージャを利用したファイル読み込み
- createfont/    InputCheckで表示しているフォント作成(本書中には登場しない)

### ch08/
- CrashApp/  故意にクラッシュするアプリ(CheckJni向け)

### ch09/
- CpuInfo/   アプリが動作しているCPU情報を取得、表示

### ch11/
- NeonApp/   NEON命令を利用した場合、しなかった場合を比較する

### ch12/
- CubeDroidTex/   libpngを利用してpng画像をテクスチャとした立方体を表示
- NativeLua/      libluaを組み込んだアプリ
- libpng-android/   androidむけにビルドしたlibpng
- lua/              android向けにビルドしたLua

## ライセンス

ここのコードはすべてApache License 2.0とします。

## NOTICE

このプロジェクトには部分的に、Android Open Source Projectのコードを含んでいます。

This product includes software developed as part of
The Android Open Source Project (http://source.android.com).

