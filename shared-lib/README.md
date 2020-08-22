# 共有ライブラリ

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは共有ライブラリの例です。
（同時に静的ライブラリも作られます。）

## 作り方

[静的ライブラリ](../static-lib)と同じところは省略しています。

### 各種ファイルを用意して配置

#### [README.md](./README.md)

#### [LICENSE](./LICENSE)

#### [autogen.sh](./autogen.sh)

#### [Makefile.am](./Makefile.am)

#### [src/Makefile.am](./src/Makefile.am)

ソースファイルディレクトリの Makefile の元になるファイルです。

```Makefile
lib_LTLIBRARIES = libhello-world.la
libhello_world_la_SOURCES = hello-world.cc hello-world.hh

pkginclude_HEADERS = hello-world.hh

libhello_world_la_LDFLAGS = -no-undefined
libhello_world_la_LDFLAGS += -version-info 2:1:0
```

* `lib_LTLIBRARIES`
    * `lib` は `/usr/lib` へインストールするものを意味し
      （`pkglib` だと `/usr/lib/パッケージ名` へのインストールになる）、
      `LTLIBRARIES` はソースファイルから libtool でビルドするライブラリ
      を意味します。
      `libhello-world.la` という名前のライブラリをビルドしてインストールする
      という指定になっています。
      libtool でビルドするライブラリの名前は lib で始まって .la
      で終わるようにします。
* `libhello_world_la_SOURCES`
    * `libhello_world_la` は `lib_LTLIBRARIES` で指定したライブラリ
      `libhello-world.la` を意味し（ハイフン `-` やピリオド `.` などは
      アンダーバー `_` に変えます）、
      `SOURCES` はビルドするために必要なソースを意味します。
      各ソースファイルの拡張子によって、
      どうやってビルドするか自動的に決定されます。
* `pkginclude_HEADERS`
* `libhello_world_la_LDFLAGS`
    * `libhello_world_la` は上記の通りで、
      `LDFLAGS` はリンク時のフラグを意味します。
      今回は libtool でリンクするため、libtool に渡すフラグとなります。
        * `-no-undefined`
            * 一部のアーキテクチャ（Windows やその他にもいくつかある模様）
              は共有ライブラリに未定義のシンボルがあることを許しません。
              `-no-undefined` は共有ライブラリを作成する際に、
              未定義のシンボルは無いことをリンカに示す宣言です。
              これらのアーキテクチャでは、これを付けておかないと
              libtool が共有ライブラリを作らなくなります。
        * `-version-info`
              共有ライブラリのバージョンを付与しています。
              バージョンの振り方は libtool の解説等をご覧ください。

Windows 環境で DLL を作る場合、
LDFLAGS に `-mwindows` を指定することがあります。
これは `gcc -dumpspecs` してみるとわかるように、
`--subsystem windows` と `-lgdi32 -lcomdlg32` が付けられることを意味します。
実行ファイルの場合はサブシステムが Windows なのか Console なのかによって
大きな違いがありますが、DLL の場合は特に違いがあるわけではなさそうです。

#### [src/hello-world.cc](./src/hello-world.cc), [src/hello-world.hh](./src/hello-world.hh)

基本的に[静的ライブラリ](../static-lib/)の例と同じです。
ただし、Windows 環境で共有ライブラリ (DLL) を作成する際に、
libtool はエクスポートするものに `__declspec(dllexport)`
が付いていることを前提とします。
そこで、マクロ `HELLO_WORLD_DLL` を用意して、
Windows かつ共有ライブラリのビルド時には `__declspec(dllexport)` に、
Windows かつ共有ライブラリ利用時には `__declspec(dllimport)` に、
その他の環境では空になるようにして、使っています。

#### [.gitignore](./.gitignore), [m4/.gitignore](./m4/.gitignore)

#### [clean.sh](./clean.sh)

### autoscan を実行

ファイルを配置したら、autoscan を実行します。

```
$ autoscan
```

すると、
[configure.scan](./configure.scan)ができます。
本来は次の手順で configure.ac を作成すれば configure.scan は不要ですが、
本 Git リポジトリでは参考にできるようわざと残してあります。

### configure.ac を作成

configure.scan を
[configure.ac](./configure.ac) にコピーして編集します。
差分は以下の通りです。

```diff
--- configure.scan
+++ configure.ac
@@ -2,14 +2,18 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([libhello-world], [2.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])
 AC_CONFIG_SRCDIR([src/hello-world.cc])
-AC_CONFIG_HEADERS([config.h])
+AC_CONFIG_HEADERS([src/config.h])
+AC_CONFIG_MACRO_DIRS([m4])

 # Checks for programs.
 AC_PROG_CXX
+AM_PROG_AR

 # Checks for libraries.
+LT_INIT([win32-dll])

 # Checks for header files.

```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AC_CONFIG_HEADERS`
* `AC_CONFIG_MACRO_DIRS`
    * libtoolize が m4 マクロを収集してくるディレクトリを指定します。
      無いと autogen.sh 実行時に libtool (libtoolize)
      が追加を検討するよう言ってきます。
* `AM_PROG_AR`
    * ライブラリ生成時に必要となります。
      無いと autogen.sh 実行時に automake が追加せよと言ってきます。
* `LT_INIT`
    * libtool を使うため追加します。
      AM_PROG_AR の後にしないと autogen.sh 実行時に警告が出ます。
      オプションは Windows 環境だったら DLL を作ることを指定しています。

### autogen.sh を実行

configure.ac ができたら、autogen.sh を実行します。

```
$ ./autogen.sh
```

これにより、configure スクリプトや Makefile.in などが作られます。

### build ディレクトリを掘り、その中で configure して make

configure スクリプトができたら、
ソースとは別のディレクトリを掘ってビルドします。

```
$ mkdir build
$ cd build
$ ../configure --prefix=/usr
$ make
```

make に成功したら、ライブラリができています。

### make install （DESTDIR 付き）

ビルドできたらインストールしてみます。
make install に DESTDIR を付けることで、
本来のインストール先とは別のディレクトリへ
一時的なインストールをします。

```
$ make install DESTDIR=`pwd`/tmp
```

ここで、libtool が、

```
libtool: warning: remember to run 'libtool --finish /usr/lib'
```

のような警告を出してきますが、これは configure で指定した prefix と
DESTDIR で指定したインストール先が異なるからです。
（共有ライブラリは prefix で指定された本来置いておく場所とは
異なる場所に置くと正常に動作しないことがあるのでこういった警告が出ます。
今回はインストールのテストをしたいだけなので問題ありません。
また、DESTDIR はバイナリパッケージ化する際に
一旦置いておく場所として指定されることが多く、
最終的には本来の prefix にインストールされて使うことになるので、
その場合にも問題ありません。）

カレントディレクトリにできた `tmp` の下を見て、
うまくインストールできたか確認します。
（共有ライブラリがどのような形でビルド・インストールされるかは、
環境によって異なります。）

### make dist

パッケージのアーカイブを作成します。

```
$ make dist
```

`libhello-world-2.0.0.tar.gz` ができるので、
中身を見て必要なファイルが含まれているか確認します。

### アーカイブからのビルドとインストール確認

アーカイブが正常に機能するか確認します。
解凍して、ビルドとインストールをしてみます。

```
$ tar xfvz libhello-world-2.0.0.tar.gz
$ cd libhello-world-2.0.0
$ mkdir build
$ cd build
$ ../configure --prefix=/usr
$ make
$ make install DESTDIR=`pwd`/tmp
```

`tmp` の下を見て、うまくインストールできたか確認します。

## ライセンス

Copyright (C) 2020 Masamichi Hosoda

ライセンスは BSD 2-Clause です。
[LICENSE](./LICENSE)をご覧ください。
