C/C++ to CFG
~~~~~~~~~~~~

``cpp2cfg`` allows you to compile any C/C++ code into LLVM IR in form of CFG with metadata.
This format is very convenient to further processing such as code coverage and generating tests.

Build
-----

This tool was written using libtooling, therefor depends on clang(-3.8) and llvm(-3.8).
The clang recommended way is to compile clang by yourself. Fortunately this can be avoided
by using system packages if you are using basically any mainstream linux distribution.

Required packages are:

    - g++
    - clang-3.8
    - llvm
    - llvm-3.8
    - llvm-3.8-dev
    - libclang-3.8-dev
    - libz-dev

Makefile uses command ``llvm-config`` from ``llvm-3.8-dev`` package.
If you compiled clang by yourself, you need to change path in Makefile to correct ``llvm-config``!

If you are using any Red Hat distribution (such as Fedora, RHEL or CentOS) you must also
ensure that there is a line ``/usr/local/lib`` in ``/etc/ld.so.conf``.
If you made a change, you need to run ``# ldconfig``.

Execution
---------

This tool takes same parameters as ``clang`` command when compiling c/c++ code.
Unfortunately at this point you must add ``-O0 -g`` flags to get source code location.
Those flags will be made implicit soon.

example:

    ``./build/cfg-gen  tests/2test.c -- -std=c99 -I /usr/lib/llvm-3.8/lib/clang/3.8.0/include -O0 -g``

TODOs:
------

    - make flags ``-O0 -g`` implicit
    - dump ``raw_llvm`` correctly (URGENT)
    - get file name from DebugLoc (URGENT)
    - propagate ``ploc`` to BB and CFG (URGENT)
    - entry and exit points in CFG (URGENT)
    - entry and exit points in BB (URGENT)
    - generate UUID (HIGH PRIORITY)
    - specify output stream
    - format output json
    - allow tool to get cmd line parameters
    - add help to tool

