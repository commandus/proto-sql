SUBDIRS = .
AM_CPPFLAGS = -I. -I/usr/local/include
bin_PROGRAMS=protosql
protosql_SOURCES= \
	sql_code_generator.cpp	proto-sql.cpp
protosql_LDADD = -L/usr/local/lib -lprotobuf -lprotoc
