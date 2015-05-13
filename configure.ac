AC_PREREQ([2.63])
AC_INIT([protosql], [0.1], [support@commandus.com])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([proto-sql.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([google/protobuf/compiler/plugin.h google/protobuf/descriptor.pb.h])

# Checks for libraries.
AC_CHECK_LIB(protobuf, main, [], AC_MSG_ERROR([Could not find libprotobuf3. Try $ ./configure LDFLAGS='-Lyour-protobuf3-lib-path']))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_OUTPUT
