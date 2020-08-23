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
pkglisp_LISP = hello.el
EXTRA_DIST = hello.el
```

* `pkglisp_LISP`
    * `pkglisp` はインストール先を Makefile 変数 `pkglispdir`
      で指定するカスタム設定を意味し
      （`pkglisp` ではなくて `lisp` を使うと、後述の AM_PATH_LISPDIR
      で設定される Makefile 変数 `lispdir` に格納される
      `/usr/share/emacs/site-lisp` などの site-lisp
      ディレクトリへのインストールを意味します）
      `LISP` は Emacs Lisp を意味します。
      `hello.el` という名前の Emacs Lisp ソースファイルと、
      それをバイトコンパイルした `hello.elc` の両方をインストールする
      指定になっています。
* `EXTRA_DIST`
    * `*_LISP` に指定したファイルは
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
@@ -2,9 +2,17 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([hello-elisp], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])

 # Checks for programs.
+AM_PATH_LISPDIR
+AC_ARG_WITH([pkglispdir],
+  AS_HELP_STRING([--with-pkglispdir=DIR],
+    [where to install lisp files (default lispdir/PACKAGE)]),
+  [pkglispdir=${withval}], [pkglispdir='${lispdir}/${PACKAGE}'])
+AC_SUBST([pkglispdir])
+AC_MSG_NOTICE([where to install lisp files is ${pkglispdir}])

 # Checks for libraries.

```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AM_PATH_LISPDIR`
    * バイトコンパイルする Emacs と、Emacs Lisp のインストール先を探します。
      Emacs Lisp のインストール先は Makefile 変数 `lispdir` に格納されます。
      `lispdir` をそのまま使うとインストール先が `/usr/share/emacs/site-lisp`
      などのようなパッケージ名が付かないものになりますが、
      パッケージ名が付いたものにしたいので `lispdir` ではなく、
      カスタム設定を使うことにします。
* `AC_ARG_WITH`
    * カスタム設定する Emacs Lisp のインストール先を configure
      スクリプトのコマンドラインオプションで指定できるようにします。
        * `[pkglispdir]`
            * オプション名を `--with-pkglispdir` にします。
        * `AS_HELP_STRING`
            * configure スクリプトを `--help` つきで起動したときの
              ヘルプメッセージの内容（オプション名と説明）を指定します。
        * `[pkglispdir=${withval}]`
            * オプションが指定された場合にその内容を変数 `pkglispdir`
              に格納します。シングルクォート無しなので変数展開されます。
              （`withval` はスクリプト内で使いまわされる変数なので
              展開が必要です。）
        * `[pkglispdir='${lispdir}/${PACKAGE}']`
            * オプションが指定されなかった場合に、AM_PATH_LISPDIR
              で得られた変数 `lispdir` にパッケージ名を付けたものを、
              変数 `pkglispdir` に設定します。
              シングルクォート付きなので変数展開されずに
              そのままの内容で設定されます。
              （Makefile 内にそのまま設定され、make 実行時に展開されます。）
* `AC_SUBST`
    * 変数 `pkglispdir` を Makefile で使用できる Makefile 変数にします。
      `lispdir` は AM_PATH_LISPDIR の中で既に AC_SUBST されているので、
      重ねて AC_SUBST する必要はありません。
* `AC_MSG_NOTICE`
    * configure スクリプト実行時に、上記で決定された最終的な
      Emacs Lisp のインストール先 `pkglispdir` を表示します。

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
