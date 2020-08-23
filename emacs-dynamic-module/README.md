# GNU Emacs ダイナミックモジュール

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは GNU Emacs ダイナミックモジュールの例です。

## 作り方

[共有ライブラリ](../shared-lib)や[Emacs Lisp](../emacs-lisp)
と同じところは省略しています。

### 各種ファイルを用意して配置

#### [README.md](./README.md)

#### [LICENSE](./LICENSE)

#### [autogen.sh](./autogen.sh)

#### [Makefile.am](./Makefile.am)

#### [src/Makefile.am](./src/Makefile.am)

ソースファイルディレクトリの Makefile の元になるファイルです。

```Makefile
noinst_LTLIBRARIES = hello-module.la
hello_module_la_SOURCES = hello-module.c

pkglisp_DATA = $(objdir)/hello-module$(SHREXT)

$(objdir)/hello-module$(SHREXT): hello-module.la

hello_module_la_LDFLAGS = -no-undefined
hello_module_la_LDFLAGS += -module
hello_module_la_LDFLAGS += -avoid-version
hello_module_la_LDFLAGS += -rpath $(pkglispdir)
```

* `noinst_LTLIBRARIES`
    * `noinst` はどこにもインストールしない（ビルド中の使用のみ）
      ことを意味し、
      `LTLIBRARIES` はソースファイルから libtool でビルドする
      ライブラリ（モジュール）を意味します。
      `hello-module.la` という名前のモジュールをビルドする
      という指定になっています。
      libtool でビルドするモジュールの名前は .la で終わるようにします。
* `hello_module_la_SOURCES`
    * `hello_module_la` は `noinst_LTLIBRARIES` で指定したモジュール
      `hello-module.la` を意味し（ハイフン `-` やピリオド `.` などは
      アンダーバー `_` に変えます）、
      `SOURCES` はビルドするために必要なソースを意味します。
      各ソースファイルの拡張子によって、
      どうやってビルドするか自動的に決定されます。
* `pkglisp_DATA`
    * `pkglisp` はインストール先を Makefile 変数 `pkglispdir`
      で指定するカスタム設定を意味し
      （`pkglisp` ではなくて `lisp` を使うと、後述の AM_PATH_LISPDIR
      で設定される Makefile 変数 `lispdir` に格納される
      `/usr/share/emacs/site-lisp` などの site-lisp
      ディレクトリへのインストールを意味します）
      `DATA` はファイルをそのまま（何もせずに）
      インストールすることを意味します。
      libtool が hello-module.la をビルドすると `$(objdir)` に拡張子
      `$(SHREXT)` の共有ライブラリ（モジュール）ができるので、
      それをインストールするように指定しています。
* `$(objdir)/hello-module$(SHREXT): hello-module.la`
    * Makefile のルールで、依存関係の連鎖を繋ぐためのものです。
      ターゲットに `pkglisp_DATA` で指定した共有ライブラリのファイル名を、
      依存関係に `hello-module.la` を指定して、共有ライブラリを作るには
      `hello-module.la` が必要であることを示しています。
      実際には libtool が `hello-module.la` をビルドすると、
      同時に共有ライブラリもビルドされるため、作成コマンドは空にしています。
* `hello_module_la_LDFLAGS`
    * `hello_module_la` は上記の通りで、
      `LDFLAGS` はリンク時のフラグを意味します。
      今回は libtool でリンクするため、libtool に渡すフラグとなります。
        * `-no-undefined`
            * 一部のアーキテクチャ（Windows やその他にもいくつかある模様）
              は共有ライブラリに未定義のシンボルがあることを許しません。
              `-no-undefined` は共有ライブラリを作成する際に、
              未定義のシンボルは無いことをリンカに示す宣言です。
              これらのアーキテクチャでは、これを付けておかないと
              libtool が共有ライブラリを作らなくなります。
        * `-module`
            * 作成する共有ライブラリをモジュールとして使用し、
              名前が `lib` で始まらないことを許容することを指定しています。
        * `-avoid-version`
            * 共有ライブラリにバージョンを付与しないことを指定しています。
        * `-rpath`
            * libtool で共有ライブラリをビルドする際に指定する必要がある、
              共有ライブラリのインストール先ディレクトリです。
              `lib_LTLIBRARIES` や `pkglib_LTLIBRARIES`
              でビルドする際には automake が自動的に付与してくれるので
              指定する必要はありませんが、`noinst_LTLIBRARIES` の場合、
              automake はインストール先を知らないので自動的には付きません。
              libtool は、これがないと共有ライブラリではなく
              静的ライブラリをビルドしてしまうため、明示的に付けておきます。

#### [src/hello-module.c](./src/hello-module.c)

ソースファイルです。
[共有ライブラリ](../shared-lib/)の例と同じく
Windows 環境を考慮してマクロ `HELLO_WORLD_DLL` を使っています。

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
--- configure.scan      2020-08-23 13:17:03.081376100 +0900
+++ configure.ac        2020-08-23 13:27:44.005930400 +0900
@@ -2,16 +2,34 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([hello-emacs-module], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])
 AC_CONFIG_SRCDIR([src/hello-module.c])
-AC_CONFIG_HEADERS([config.h])
+AC_CONFIG_HEADERS([src/config.h])
+AC_CONFIG_MACRO_DIRS([m4])

 # Checks for programs.
-AC_PROG_CC
+AC_PROG_CC_C99
+AS_IF([test "x$ac_cv_prog_cc_c99" = xno],
+  AC_MSG_ERROR([C99 compiler is not found.]))
+AM_PROG_AR
+AM_PATH_LISPDIR
+AC_ARG_WITH([pkglispdir],
+  AS_HELP_STRING([--with-pkglispdir=DIR],
+    [where to install emacs dynamic modules (default lispdir/PACKAGE)]),
+  [pkglispdir=${withval}], [pkglispdir='${lispdir}/${PACKAGE}'])
+AC_SUBST([pkglispdir])
+AC_MSG_NOTICE([where to install emacs dynamic modules is ${pkglispdir}])

 # Checks for libraries.
+LT_INIT([disable-static shared win32-dll])
+SHREXT=${shrext_cmds}
+AC_SUBST([SHREXT])
+AC_SUBST([objdir])

 # Checks for header files.
+AC_CHECK_HEADERS([emacs-module.h],,
+  AC_MSG_ERROR([emacs-module.h is not found.]))
 AC_CHECK_HEADERS([string.h])

 # Checks for typedefs, structures, and compiler characteristics.
```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AC_CONFIG_HEADERS`
* `AC_CONFIG_MACRO_DIRS` →[共有ライブラリ](../shared-lib/)参照
* `AC_PROG_CC_C99`
    * Emacs ダイナミックモジュールには C99 コンパイラが必要なため。
* `AS_IF`
    * C99 コンパイラが見つからなかったらエラーメッセージを表示します。
* `AM_PROG_AR` →[共有ライブラリ](../shared-lib/)参照
* `AM_PATH_LISPDIR` →[Emacs Lisp](../emacs-lisp/)参照
* `AC_ARG_WITH` →[Emacs Lisp](../emacs-lisp/)参照
* `AC_SUBST([pkglispdir])` →[Emacs Lisp](../emacs-lisp/)参照
* `AC_MSG_NOTICE` →[Emacs Lisp](../emacs-lisp/)参照
* `LT_INIT`
    * `disable-static shared`
        * モジュールは共有ライブラリでなければならないため、
          静的ライブラリのビルドをせず共有ライブラリのみビルドします。
    * `win32-dll` →[共有ライブラリ](../shared-lib/)参照
* `SHREXT`
    * `LT_INIT` 内部で得られた共有ライブラリの拡張子を変数 `SHREXT`
      に格納します。
* `AC_SUBST([SHREXT])`
    * 変数 `SHREXT` を Makefile から使える Makefile 変数にします。
      共有ライブラリの拡張子は環境によって `.so` だったり `.dll`
      だったりするので、これらの違いが反映できるようにします。
* `AC_SUBST([objdir])`
    * libtool が共有ライブラリをビルドしたとき、
      共有ライブラリ本体が格納されるディレクトリ名 `objdir` を
      Makefile 変数にします。
* `AC_CHECK_HEADERS`
    * Emacs ダイナミックモジュール用のヘッダファイルを確認して、
      なければエラーメッセージを表示します。

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

カレントディレクトリにできた `tmp` の下を見て、
うまくインストールできたか確認します。
（モジュールがどのような形でビルド・インストールされるかは、
環境によって異なります。）

### make dist

パッケージのアーカイブを作成します。

```
$ make dist
```

`hello-emacs-module-0.0.0.tar.gz` ができるので、
中身を見て必要なファイルが含まれているか確認します。

### アーカイブからのビルドとインストール確認

アーカイブが正常に機能するか確認します。
解凍して、ビルドとインストールをしてみます。

```
$ tar xfvz hello-emacs-module-0.0.0.tar.gz
$ cd hello-emacs-module-0.0.0
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
