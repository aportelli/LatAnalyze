AC_DEFUN([AX_GCC_OPTION], [
AC_REQUIRE([AC_PROG_CC])

AC_MSG_CHECKING([if gcc accepts $1 option])

AS_IF([ test "x$GCC" = "xyes" ],[
AS_IF([ test -z "$3" ],[
ax_gcc_option_test="int main()
{
return 0;
}"
],[
ax_gcc_option_test="$3"
])

# Dump the test program to file
cat <<EOF > conftest.c
$ax_gcc_option_test
EOF

# Dump back the file to the log, useful for debugging purposes
AC_TRY_COMMAND(cat conftest.c 1>&AS_MESSAGE_LOG_FD)

AS_IF([ AC_TRY_COMMAND($CC $2 $1 -c conftest.c 1>&AS_MESSAGE_LOG_FD) ],[
AC_MSG_RESULT([yes])
$4
],[
AC_MSG_RESULT([no])
$5
])
],[
AC_MSG_RESULT([no gcc available])
])
])

AC_DEFUN([AX_GCC_VERSION], [
GCC_VERSION=""
AX_GCC_OPTION([-dumpversion],[],[],[
ax_gcc_version_option=yes
],[
ax_gcc_version_option=no
])
AS_IF([test "x$GCC" = "xyes"],[
AS_IF([test "x$ax_gcc_version_option" != "xno"],[
AC_CACHE_CHECK([gcc version],[ax_cv_gcc_version],[
ax_cv_gcc_version="`$CC -dumpversion`"
AS_IF([test "x$ax_cv_gcc_version" = "x"],[
ax_cv_gcc_version=""
])
])
GCC_VERSION=$ax_cv_gcc_version
])
])
AC_SUBST([GCC_VERSION])
])

AC_DEFUN([AX_GXX_VERSION], [
GXX_VERSION=""
AX_GCC_OPTION([-dumpversion],[],[],[
ax_gcc_version_option=yes
],[
ax_gcc_version_option=no
])
AS_IF([test "x$GXX" = "xyes"],[
AS_IF([test "x$ax_gxx_version_option" != "no"],[
AC_CACHE_CHECK([gxx version],[ax_cv_gxx_version],[
ax_cv_gxx_version="`$CXX -dumpversion`"
AS_IF([test "x$ax_cv_gxx_version" = "x"],[
ax_cv_gxx_version=""
])
])
GXX_VERSION=$ax_cv_gxx_version
])
])
AC_SUBST([GXX_VERSION])
])

AC_DEFUN([AX_COMPILER_VENDOR],
[
AC_CACHE_CHECK([for _AC_LANG compiler vendor], ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor,
[ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor=unknown
# note: don't check for gcc first since some other compilers define __GNUC__
for ventest in intel:__ICC,__ECC,__INTEL_COMPILER ibm:__xlc__,__xlC__,__IBMC__,__IBMCPP__ pathscale:__PATHCC__,__PATHSCALE__ gnu:__GNUC__ sun:__SUNPRO_C,__SUNPRO_CC hp:__HP_cc,__HP_aCC dec:__DECC,__DECCXX,__DECC_VER,__DECCXX_VER borland:__BORLANDC__,__TURBOC__ comeau:__COMO__ cray:_CRAYC kai:__KCC lcc:__LCC__ metrowerks:__MWERKS__ sgi:__sgi,sgi microsoft:_MSC_VER watcom:__WATCOMC__ portland:__PGI; do
vencpp="defined("`echo $ventest | cut -d: -f2 | sed 's/,/) || defined(/g'`")"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(,[
#if !($vencpp)
thisisanerror;
#endif
])], [ax_cv_]_AC_LANG_ABBREV[_compiler_vendor=`echo $ventest | cut -d: -f1`; break])
done
])
])


