# Emacs Lisp

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは Emacs Lisp の例です。

## 作り方

[通常の実行ファイル](../executable-bin/)と同じところは省略しています。

### 各種ファイルを用意して配置

本来、ファイルは少しずつ用意し、
下記手順の実行時に出るエラーや警告を元に修正や追加をしていくのが
通常の方法と思いますが、ここでは最初から完成版を配置しています。

#### [README.md](./README.md)

#### [LICENSE](./LICENSE)

#### [autogen.sh](./autogen.sh)

#### [Makefile.am](./Makefile.am)

トップレベルの Makefile の元になるファイルです。

```Makefile
SUBDIRS = lisp
dist_doc_DATA = README.md LICENSE
EXTRA_DIST = autogen.sh

ACLOCAL_AMFLAGS = -I m4
```

* `SUBDIRS`
    * 一つ下の階層で Makefile を配置するサブディレクトリを指定します。
      今回は Emacs Lisp ファイルを配置したサブディレクトリ `lisp` だけです。
* `dist_doc_DATA`
* `EXTRA_DIST`
* `ACLOCAL_AMFLAGS`

#### [lisp/Makefile.am](./lisp/Makefile.am)

Emacs Lisp ディレクトリの Makefile の元になるファイルです。

```Makefile
lisp_LISP = hello.el
EXTRA_DIST = hello.el
```

* `lisp_LISP`
    * `lisp` は `/usr/share/emacs/site-lisp` などの
       site-lisp ディレクトリへのインストールを意味し、
      `LISP` は Emacs Lisp を意味します。
      `hello.el` という名前の Emacs Lisp ソースファイルと、
      それをバイトコンパイルした `hello.elc` の両方をインストールする
      指定になっています。
* `EXTRA_DIST`
    * `lisp_LISP` に指定したファイルは
      （実行ファイルやライブラリが含まれないのと同様）そのままでは
      パッケージのアーカイブに含まれないので、ここで追加しています。

#### [lisp/hello.el](./lisp/hello.el)

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
@@ -2,9 +2,12 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([hello-elisp], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])

 # Checks for programs.
+AM_PATH_LISPDIR
+lispdir=${lispdir}/${PACKAGE}

 # Checks for libraries.

```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AM_PATH_LISPDIR`
    * バイトコンパイルする Emacs と、Emacs Lisp のインストール先を探します。
* `lispdir`
    * そのままだと Emacs Lisp のインストール先が `/usr/share/emacs/site-lisp`
      などのようなパッケージ名が付かないものになるので、
      パッケージ名の付いたサブディレクトリ
      `/usr/share/emacs/site-lisp/パッケージ名` などのように変更します。
      （Makefile.am の中で変更するのは少々困難※なのでここで変更します。）

※ Makefile.am の中で `lispdir = $(lispdir)/$(PACKAGE)`
すると、再帰的な定義でエラーになります。
`lispdir := $(lispdir)/$(PACKAGE)` にするには AM_INIT_AUTOMAKE
に `-Wno-portability` オプションが必要になる上、
Makefile 中では完全に置き換えられてしまう（元の lispdir 定義が消える）ので、
インストール先が `/パッケージ名` になってしまい、おかしくなります。
（別の名前に保存し AC_SUBST で Makefile から使えるようにして、
Makefile.am で `lispdir := $(別の名前)/$(PACKAGE)` のようにするなど、
置き換える方法が無いわけではないですが、
そこまでするメリットはないものと思います。）

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

make に成功したら、バイトコンパイルした hello.elc ができています。

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
`tmp/usr/share/emacs/site-lisp/hello-elisp`
など（環境によって異なります）に `hello.el` と `hello.elc` が、
`tmp/usr/share/doc/hello-elisp` に `README.md` と `LICENSE` が、
それぞれインストールされているはずです。

### make dist

パッケージのアーカイブを作成します。

```
$ make dist
```

`hello-elisp-0.0.0.tar.gz` ができるので、
中身を見て必要なファイルが含まれているか確認します。

### アーカイブからのビルドとインストール確認

アーカイブが正常に機能するか確認します。
解凍して、ビルドとインストールをしてみます。

```
$ tar xfvz hello-elisp-0.0.0.tar.gz
$ cd hello-elisp-0.0.0
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
