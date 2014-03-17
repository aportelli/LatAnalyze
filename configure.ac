# Process this file with autoconf to produce a configure script.

# Initialization
AC_PREREQ([2.64])
AC_INIT([LatAnalyze],[3.0alpha1],[antonin.portelli@me.com],[LatAnalyze])
AC_CONFIG_AUX_DIR([.buildutils])
AC_CONFIG_SRCDIR([lib/Global.cpp])
AC_CONFIG_SRCDIR([utils/sample_read.cpp])
AC_CONFIG_SRCDIR([examples/exMathInterpreter.cpp])
AC_CONFIG_MACRO_DIR([.buildutils/m4])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([config.h])

# Checks for programs
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PROG_YACC
m4_ifdef([AM_PROG_AR],[AM_PROG_AR])

LT_INIT

# Options
AC_ARG_ENABLE([SSE],
	[AS_HELP_STRING([--enable-SSE],
		[compiles SSE version of ranlux random generator])],
	[AC_DEFINE([HAVE_SSE],
				[1],
				[Define to 1 if your CPU support SSE instructions.])],
	[]
)
AC_ARG_WITH([Minuit2],
    [AS_HELP_STRING([--with-Minuit2=prefix],
		[try this for a non-standard install prefix of the Minuit2 library])],
    [AM_CFLAGS="$AM_CFLAGS -I$with_Minuit2/include"]
    [AM_CXXFLAGS="$AM_CXXFLAGS -I$with_Minuit2/include"]
	[AM_LDFLAGS="$AM_LDFLAGS -L$with_Minuit2/lib"],
	[]
)

CFLAGS="$AM_CFLAGS $CFLAGS"
CXXFLAGS="$AM_CXXFLAGS $CXXFLAGS"
LDFLAGS="$AM_LDFLAGS $LDFLAGS"

# Get compilers informations
AX_COMPILER_VENDOR
AC_DEFINE_UNQUOTED([C_COMP_VENDOR],["$ax_cv_c_compiler_vendor"],
			[vendor of C compiler that will compile the code])
AM_CONDITIONAL([CC_GNU],[test $ax_cv_c_compiler_vendor = "gnu"])
AM_CONDITIONAL([CC_INTEL],[test $ax_cv_c_compiler_vendor = "intel"])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AX_COMPILER_VENDOR
AC_DEFINE_UNQUOTED([CXX_COMP_VENDOR],["$ax_cv_cxx_compiler_vendor"],
			[vendor of C++ compiler that will compile the code])
AM_CONDITIONAL([CXX_GNU],[test $ax_cv_cxx_compiler_vendor = "gnu"])
AM_CONDITIONAL([CXX_INTEL],[test $ax_cv_cxx_compiler_vendor = "intel"])
AX_GCC_VERSION
AC_DEFINE_UNQUOTED([GCC_VERSION],["$GCC_VERSION"],
			[version of gcc that will compile the code])
AX_GXX_VERSION
AC_DEFINE_UNQUOTED([GXX_VERSION],["$GXX_VERSION"],
			[version of g++ that will compile the code])

# Checks for libraries.
AC_CHECK_LIB([m],[cos],[],[AC_MSG_ERROR([libm library not found])])
SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -lMinuit2"
AC_MSG_CHECKING([for ROOT::Minuit2::BasicMinimumError in -lMinuit2]);
AC_LINK_IFELSE(
	[AC_LANG_PROGRAM([#include <Minuit2/BasicMinimumError.h>],
	[ROOT::Minuit2::BasicMinimumError dummy(0)])],
	[LIBS="$LIBS -lMinuit2"]
	[AC_DEFINE([HAVE_MINUIT2],
				[1],
				[Define to 1 if you have the `Minuit2' library (-lMinuit2).])]
    [have_minuit=true]
	[AC_MSG_RESULT([yes])],
	[have_minuit=false]
    [AC_MSG_RESULT([no])])
AM_CONDITIONAL([HAVE_MINUIT], [test x$have_minuit = xtrue])
LDFLAGS=$SAVED_LDFLAGS

# Checks for header files.
AC_HEADER_STDC

AC_SUBST([LIBS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([Makefile lib/Makefile utils/Makefile examples/Makefile])
AC_OUTPUT
