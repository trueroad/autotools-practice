# 通常の実行ファイル

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは通常の実行ファイルの例です。

## 作り方

以下、インストール先ディレクトリ名は configure スクリプト実行時に
`--prefix=/usr` を指定したときのものです。

### 各種ファイルを用意して配置

本来、ファイルは少しずつ用意し、
下記手順の実行時に出るエラーや警告を元に修正や追加をしていくのが
通常の方法と思いますが、ここでは最初から完成版を配置しています。

#### [README.md](./README.md)

このファイルです。 `/usr/share/doc/パッケージ名`
にインストールされるドキュメントのサンプルでもあります。

#### [LICENSE](./LICENSE)

ライセンスについて記載したファイルです。 `/usr/share/doc/パッケージ名`
にインストールされるドキュメントのサンプルでもあります。

#### [autogen.sh](./autogen.sh)

Git リポジトリからソース一式を入手してビルドする場合、
最初に実行すべきスクリプトとして用意しました。
Autotools により configure スクリプトなどの生成をします。

```sh
#!/bin/sh

autoreconf -v -i
```

このサンプルでは非常に単純で、autoreconf を実行しているだけです。
`-v` は verbose で `-i` は必要なファイルが無ければコピーしてくる
というオプションです。

OSS プロジェクトによって、こうしたスクリプトを用意していなかったり、
同様の目的のものが別の名前になっていたり、
まちまちで特に標準的なものではありません。
パッケージのアーカイブからビルドする場合は、生成済みの configure
スクリプトなどが含まれるため、実行する必要はありません。
（ただし、アーカイブ作成時に使用した Autotools
のバージョンが古くて問題になるような環境では
実行した方がいい場合もあり得るため、収録するようにしています。）

#### [Makefile.am](./Makefile.am)

トップレベルの Makefile の元になるファイルです。
autogen.sh を実行すると automake によって
Makefile.am から Makefile.in が生成され、
さらに configure スクリプトを実行すると
Makefile.in から Makefile が生成されます。

```Makefile
SUBDIRS = src
dist_doc_DATA = README.md LICENSE
EXTRA_DIST = autogen.sh

ACLOCAL_AMFLAGS = -I m4
```

* `SUBDIRS`
    * 一つ下の階層で Makefile を配置するサブディレクトリを指定します。
      今回はソースファイルを配置したサブディレクトリ `src` だけです。
      サブディレクトリ内には、そこでも automake を使うなら Makefile.am を、
      automake を使わずに autoconf だけ使うなら Makefile.in を、
      automake も autoconf も使わないなら Makefile を配置します。
* `dist_doc_DATA`
    * `/usr/share/doc/パッケージ名` にインストールする
      ドキュメントのファイルを指定します。
      README.md と LICENSE を指定しています。
* `EXTRA_DIST`
    * 追加でパッケージのアーカイブに含めるファイルを指定します。
      autogen.sh を指定しています。
* `ACLOCAL_AMFLAGS`
    * autogen.sh 実行時に呼ばれる aclocal のオプションです。
      m4 マクロのファイルを格納するディレクトリを指定しています。
      今回は事前に用意するものは無いのでディレクトリは空です。

#### [src/Makefile.am](./src/Makefile.am)

ソースファイルディレクトリの Makefile の元になるファイルです。
ここでも automake を使うので Makefile.am としています。

```Makefile
bin_PROGRAMS = hello-world
hello_world_SOURCES = main.cc sub.cc sub.hh
```

* `bin_PROGRAMS`
    * `bin` は `/usr/bin` へインストールするものを意味し、
      `PROGRAMS` はソースファイルからビルドするプログラム
      （ライブラリやスクリプトではないもの）を意味します。
      `hello-world` という名前の実行ファイルをビルドしてインストールする
      という指定になっています。
      複数の実行ファイルをビルド・インストールするのであれば、
      ここに複数並べます。
* `hello_world_SOURCES`
    * `hello_world` は `bin_PROGRAMS` で指定した実行ファイル
      `hello-world` を意味し（ハイフン `-` やピリオド `.` などは
      アンダーバー `_` に変えます）、
      `SOURCES` はビルドするために必要なソースを意味します。
      各ソースファイルの拡張子によって、
      どうやってビルドするか自動的に決定されます。
      複数の実行ファイルをビルドする場合には、
      `hello_world` の部分の名前をそれぞれの実行ファイルの名前に変え、
      それらの変数にそれぞれの実行ファイルのビルドに必要なソースを指定します。

#### [src/main.cc](./src/main.cc), [src/sub.cc](./src/sub.cc), [src/sub.hh](./src/sub.hh)

ソースファイルです。

#### [.gitignore](./.gitignore), [m4/.gitignore](./m4/.gitignore)

自動生成されるファイルなど
Git リポジトリに収録したくないファイル名の一覧です。
Autotools とは関係ありません。
.gitignore 自体は Git リポジトリに収録しますが、Autotools
で生成するパッケージのアーカイブには収録しないようにしています。

#### [clean.sh](./clean.sh)

簡単に最初からやり直しができるように
autogen.sh などで自動生成したファイルを消すスクリプトを
用意してみました。普通はこういうものはありませんし、
パッケージのアーカイブにも収録されないようにしています。

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
@@ -2,9 +2,10 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([hello-world], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])
 AC_CONFIG_SRCDIR([src/main.cc])
-AC_CONFIG_HEADERS([config.h])
+AC_CONFIG_HEADERS([src/config.h])

 # Checks for programs.
 AC_PROG_CXX
```

* `AC_INIT`
    * パッケージ名、バージョン、メールアドレスを設定しています。
* `AM_INIT_AUTOMAKE`
    * automake を使うため追加します。
      オプションで、全警告表示、警告をエラーとして扱う、
      GNU 規約に厳密に従わない（COPYING や NEWS などがなくてもよい）
      ことを指定しています。
* `AC_CONFIG_HEADERS`
    * 生成する config.h のディレクトリをトップからソースに移しています。

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

make に成功したら、実行ファイル hello-world ができています。

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
`tmp/usr/bin` に `hello-world` が、
`tmp/usr/share/doc/hello-world` に `README.md` と `LICENSE` が、
それぞれインストールされているはずです。

### make dist

パッケージのアーカイブを作成します。

```
$ make dist
```

`hello-world-0.0.0.tar.gz` ができるので、
中身を見て必要なファイルが含まれているか確認します。

### アーカイブからのビルドとインストール確認

アーカイブが正常に機能するか確認します。
解凍して、ビルドとインストールをしてみます。

```
$ tar xfvz hello-world-0.0.0.tar.gz
$ cd hello-world-0.0.0
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
