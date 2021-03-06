AC_DEFUN([JW_LIB_PREFIX_SUFFIX],
[case "$host_os" in
    mingw32*)
        JNHW_LIB_SUFFIX="-\$(JNHW_VERSION).dll"
        JNHW_LIB_PREFIX="lib"
        JNHW_STRIP_FLAG="-s"
    ;;
    darwin)
        JNHW_LIB_SUFFIX=".\$(JNHW_VERSION).dylib"
        JNHW_LIB_PREFIX="lib"
        JNHW_STRIP_FLAG="-S"
    ;;
    *)
        JNHW_LIB_PREFIX="lib"
        JNHW_LIB_SUFFIX=".so.\$(JNHW_VERSION)"
        JNHW_STRIP_FLAG="-s"
    ;;
esac

AC_SUBST(JNHW_LIB_PREFIX)
AC_SUBST(JNHW_LIB_SUFFIX)
AC_SUBST(JNHW_STRIP_FLAG)
])
