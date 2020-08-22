# 静的ライブラリ

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは静的ライブラリ（スタティックライブラリ）の例です。

## 作り方

[通常の実行ファイル](../executable-bin)と同じところは省略しています。

### 各種ファイルを用意して配置

#### [README.md](./README.md)

#### [LICENSE](./LICENSE)

#### [autogen.sh](./autogen.sh)

#### [Makefile.am](./Makefile.am)

#### [src/Makefile.am](./src/Makefile.am)

ソースファイルディレクトリの Makefile の元になるファイルです。

```Makefile
lib_LIBRARIES = libhello-world.a
libhello_world_a_SOURCES = hello-world.cc hello-world.hh

pkginclude_HEADERS = hello-world.hh
```

* `lib_LIBRARIES`
    * `lib` は `/usr/lib` へインストールするものを意味し
      （`pkglib` だと `/usr/lib/パッケージ名` へのインストールになる）、
      `LIBRARIES` はソースファイルからビルドするライブラリ
      （ただし libtool は使わない）を意味します。
      `libhello-world.a` という名前のライブラリをビルドしてインストールする
      という指定になっています。
      静的ライブラリの名前は lib で始まって .a で終わるようにします。
* `libhello_world_a_SOURCES`
    * `libhello_world_a` は `lib_LIBRARIES` で指定したライブラリ
      `libhello-world.a` を意味し（ハイフン `-` やピリオド `.` などは
      アンダーバー `_` に変えます）、
      `SOURCES` はビルドするために必要なソースを意味します。
      各ソースファイルの拡張子によって、
      どうやってビルドするか自動的に決定されます。
* `pkginclude_HEADERS`
    * `pkginclude` は `/usr/include/パッケージ名`
      へインストールするものを意味し
      （`include` だと `/usr/include` へのインストールになる）、
      `HEADERS` はヘッダファイルを意味します。
      `hello-world.hh` というヘッダファイルをインストールする
      という指定になっています。

#### [src/hello-world.cc](./src/hello-world.cc), [src/hello-world.hh](./src/hello-world.hh)

ソースファイルです。

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
@@ -2,12 +2,15 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([libhello-world], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])
 AC_CONFIG_SRCDIR([src/hello-world.cc])
-AC_CONFIG_HEADERS([config.h])
+AC_CONFIG_HEADERS([src/config.h])

 # Checks for programs.
 AC_PROG_CXX
+AC_PROG_RANLIB
+AM_PROG_AR

 # Checks for libraries.

```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AC_CONFIG_HEADERS`
* `AC_PROG_RANLIB`
    * 静的ライブラリ生成時に必要となります。
      無いと autogen.sh 実行時に automake が追加せよと言ってきます。
* `AM_PROG_AR`
    * ライブラリ生成時に必要となります。
      無いと autogen.sh 実行時に automake が追加せよと言ってきます。

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

make に成功したら、静的ライブラリ libhello-world.a ができています。

### make install （DESTDIR 付き）

ビルドできたらインストールしてみます。
make install に DESTDIR を付けることで、
本来のインストール先とは別のディレクトリへ
一時的なインストールをします。

```
$ make install DESTDIR=`pwd`/tmp
```

カレントディレクトリにできた `tmp` の下を見て、
うまくインストールできたか確認します。
`tmp/usr/lib` に `libhello-world.a` が、
`tmp/usr/include/libhello-world` に `hello-world.hh` が、
`tmp/usr/share/doc/libhello-world` に `README.md` と `LICENSE` が、
それぞれインストールされているはずです。

### make dist

パッケージのアーカイブを作成します。

```
$ make dist
```

`libhello-world-0.0.0.tar.gz` ができるので、
中身を見て必要なファイルが含まれているか確認します。

### アーカイブからのビルドとインストール確認

アーカイブが正常に機能するか確認します。
解凍して、ビルドとインストールをしてみます。

```
$ tar xfvz libhello-world-0.0.0.tar.gz
$ cd libhello-world-0.0.0
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
