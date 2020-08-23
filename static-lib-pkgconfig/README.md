# 静的ライブラリ + pkg-config

Autotools (autoconf, automake, libtool) の使い方が、
いつもよくわからなくなってしまうので、半分自分用のメモとして
[Autotools 手習い](https://github.com/trueroad/autotools-practice)
を作ってみました。

これは[静的ライブラリ](../static-lib/)（スタティックライブラリ）
に pkg-config 対応を追加したものです。

## 作り方

[静的ライブラリ](../static-lib/)との差分だけ記載していきます。

### 各種ファイルを用意して配置

#### [src/libhello-world.pc.in](./src/libhello-world.pc.in)

pkg-config の設定ファイルのひな型を追加します。
（
[ax_create_pkgconfig_info
](https://www.gnu.org/software/autoconf-archive/ax_create_pkgconfig_info.html)
を使って生成する方法もあるようですが、
ここではひな型のファイルから生成する方法を採ります。）

```
prefix=@prefix@
exec_prefix=@exec_prefix@
libdir=@libdir@
includedir=@includedir@

Name: @PACKAGE@
Description: @PACKAGE_DESCRIPTION@
Version: @VERSION@
Libs: -L${libdir} -lhello-world
Cflags: -I${includedir}/@PACKAGE@
```

`@` で囲まれた部分が、configure スクリプトによって置き換えられて、
`src/libhello-world.pc` ファイルができます。

#### [src/Makefile.am](./src/Makefile.am)

pkg-config 設定ファイルのインストール設定を追加します。

```Makefile
lib_LIBRARIES = libhello-world.a
libhello_world_a_SOURCES = hello-world.cc hello-world.hh

pkginclude_HEADERS = hello-world.hh

pkgconfig_DATA = libhello-world.pc
```

[静的ライブラリ](../static-lib/)との差分は 1 つだけです。

* `pkgconfig_DATA`
    * `pkgconfig` は インストール先を Makefile 変数 `pkgconfigdir` で指定する
      カスタム設定を意味し、 `DATA` はファイルをそのまま
      （make では何もせずに）インストールすることを意味します。
      `libhello-world.pc` という名前の pkg-config 設定ファイルを
      インストールするという指定になっています。

### configure.ac を作成

configure.scan と
[configure.ac](./configure.ac) の差分は以下の通りです。

```diff
--- configure.scan
+++ configure.ac
@@ -2,12 +2,25 @@
 # Process this file with autoconf to produce a configure script.

 AC_PREREQ([2.69])
-AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
+AC_INIT([libhello-world], [0.0.0], [test@example.org])
+AM_INIT_AUTOMAKE([-Wall -Werror foreign])
 AC_CONFIG_SRCDIR([src/hello-world.cc])
-AC_CONFIG_HEADERS([config.h])
+AC_CONFIG_HEADERS([src/config.h])
+
+PACKAGE_DESCRIPTION="Autotools practice for a static library with pkg-config"
+AC_SUBST([PACKAGE_DESCRIPTION])
+
+AC_ARG_WITH([pkgconfigdir],
+  AC_HELP_STRING([--with-pkgconfigdir=DIR],
+    [where to install pkgconfig file (default libdir/pkgconfig)]),
+  [pkgconfigdir=${withval}], [pkgconfigdir='${libdir}/pkgconfig'])
+AC_SUBST([pkgconfigdir])
+AC_MSG_NOTICE([where to install pkgconfig file is ${pkgconfigdir}])

 # Checks for programs.
 AC_PROG_CXX
+AC_PROG_RANLIB
+AM_PROG_AR

 # Checks for libraries.

@@ -18,5 +31,6 @@
 # Checks for library functions.

 AC_CONFIG_FILES([Makefile
-                 src/Makefile])
+                 src/Makefile
+                 src/libhello-world.pc])
 AC_OUTPUT
```

* `AC_INIT`
* `AM_INIT_AUTOMAKE`
* `AC_CONFIG_HEADERS`
* `PACKAGE_DESCRIPTION`
    * `libhello-world.pc` の Description に設定する文字列を指定します。
* `AC_SUBST([PACKAGE_DESCRIPTION])`
    * `PACKAGE_DESCRIPTION` が `libhello-world.pc`
      で置き換えられるようにします。
* `AC_ARG_WITH`
    * カスタム設定する `libhello-world.pc` のインストール先を configure
      スクリプトのコマンドラインオプションで指定できるようにします。
        * `[pkgconfigdir]`
            * オプション名を `--with-pkgconfigdir` にします。
        * `AS_HELP_STRING`
            * configure スクリプトを `--help` つきで起動したときの
              ヘルプメッセージの内容（オプション名と説明）を指定します。
        * `[pkgconfigdir=${withval}]`
            * オプションが指定された場合にその内容を変数 `pkgconfigdir`
              に格納します。シングルクォート無しなので変数展開されます。
              （`withval` はスクリプト内で使いまわされる変数なので
              展開が必要です。）
        * `[pkgconfigdir='${libdir}/pkgconfig']`
            * オプションが指定されなかった場合に、`libdir`
              に `pkgconfig` を付けたものを、変数 `pkgconfigdir`
              に設定します。
              シングルクォート付きなので変数展開されずに
              そのままの内容で設定されます。
              （Makefile 内にそのまま設定され、make 実行時に展開されます。）
* `AC_SUBST([pkgconfigdir])`
    * 変数 `pkgconfigdir` を Makefile で使用できる Makefile 変数にします。
* `AC_MSG_NOTICE`
    * configure スクリプト実行時に、上記で決定された最終的な
      `libhello-world.pc` のインストール先 `pkgconfigdir` を表示します。
* `AC_PROG_RANLIB`
* `AM_PROG_AR`
* `AC_CONFIG_FILES`
    * pkg-config の設定ファイルのひな型 `libhello-world.pc.in` から
      pkg-config の設定ファイル `libhello-world.pc` の生成を追加します。

## ライセンス

Copyright (C) 2020 Masamichi Hosoda

ライセンスは BSD 2-Clause です。
[LICENSE](./LICENSE)をご覧ください。
