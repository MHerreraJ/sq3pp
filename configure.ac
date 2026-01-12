#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sq3pp], [1.0.2], [])
AC_CONFIG_SRCDIR([src/Database.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Support for cross-compilation
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])
AM_PROG_AR

# Verify cross-compilation tools exist when host is specified
if test "$build" != "$host"; then
    AC_MSG_CHECKING([if $CXX can compile for $host])
    
    # Try to compile a simple test
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
        [
            # Check if compiler target matches host
            AC_MSG_RESULT([checking target])
            compiler_target=$($CXX -dumpmachine 2>/dev/null || echo "unknown")
            AC_MSG_CHECKING([compiler target])
            AC_MSG_RESULT([$compiler_target])
            
            if test "$compiler_target" = "unknown"; then
                AC_MSG_ERROR([Cannot determine compiler target. Cross-compiler may not be in PATH.])
            fi
            
            if test "$compiler_target" != "$host"; then
                AC_MSG_ERROR([Compiler target ($compiler_target) does not match host ($host). Please set correct cross-compiler via CC/CXX variables.])
            fi
        ],
        [
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([Cannot compile for $host. Compiler $CXX not found or not working. Check --host settings.])
        ])
fi

# Enable libtool
LT_PREREQ([2.2])
LT_INIT([shared static])

# SQLite3 environment variables
AC_ARG_VAR([LIBSQLITE3_CFLAGS], [C/C++ compiler flags for SQLite3])
AC_ARG_VAR([LIBSQLITE3_LIBS], [linker flags for SQLite3])

# Add SQLite3 flags to build variables
CXXFLAGS="$CXXFLAGS $LIBSQLITE3_CFLAGS"
CFLAGS="$CFLAGS $LIBSQLITE3_CFLAGS"

# Only check for SQLite3 if not explicitly provided
if test -z "$LIBSQLITE3_LIBS"; then
    # Checks for libraries.
    AC_CHECK_LIB([sqlite3], [sqlite3_open], [LIBSQLITE3_LIBS="-lsqlite3"], [
        AC_MSG_ERROR([SQLite3 library not found. Please install libsqlite3-dev or set LIBSQLITE3_LIBS])
    ])
    
    # Checks for header files.
    AC_CHECK_HEADERS([sqlite3.h], [], [
        AC_MSG_ERROR([SQLite3 headers not found. Please install libsqlite3-dev or set LIBSQLITE3_CFLAGS])
    ])
else
    AC_MSG_NOTICE([Using user-provided SQLite3 configuration: $LIBSQLITE3_LIBS])
fi

LDFLAGS="$LDFLAGS $LIBSQLITE3_LIBS"
LIBS="$LIBS $LIBSQLITE3_LIBS"

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memcpy strcpy strlen])

# Check for C++17 support
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

# Option to build examples
AC_ARG_ENABLE([examples],
    [AS_HELP_STRING([--enable-examples], [build example programs (default: no)])],
    [enable_examples=$enableval],
    [enable_examples=no])
AM_CONDITIONAL([BUILD_EXAMPLES], [test "x$enable_examples" = "xyes"])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    example/Makefile
])

AC_OUTPUT

echo "
  $PACKAGE_NAME $PACKAGE_VERSION configuration:
  ================================

  Host:            $host
  Build:           $build
  Compiler:        $CXX
  CXXFLAGS:        $CXXFLAGS
  LDFLAGS:         $LDFLAGS
  LIBSQLITE3_CFLAGS:  $LIBSQLITE3_CFLAGS
  LIBSQLITE3_LIBS:    $LIBSQLITE3_LIBS
  prefix:          $prefix
"
