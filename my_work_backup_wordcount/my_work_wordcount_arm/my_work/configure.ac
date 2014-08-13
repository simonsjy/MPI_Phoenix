#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.1])
AC_INIT(my_work, 1.0, kuangrenyx@qq.com)
AM_INIT_AUTOMAKE(my_work, 1.0, kuangrenyx@qq.com)
#AC_CONFIG_SRCDIR([src/testprog/main.cpp])
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([pthread], [main])

AC_HEADER_STDBOOL

AC_C_INLINE

AC_TYPE_SIZE_T

# Checks for header files.
AC_CHECK_HEADERS()

AC_PROG_RANLIB
# Checks for library functions.

AC_OUTPUT([Makefile
	src/Makefile
	test/Makefile
	misc/www/Makefile
	misc/mod/Makefile
	dir/Makefile
	    ])
