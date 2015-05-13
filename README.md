proto-sql
=========

Protobuf protoc compiler plugin generating MySQL script useful to create backend database.

proto-sql protoc plugin is very simple, so you can use it as an example how to implement your own plugin.

Generated SQL script is intended for MySQL. For others you need make a little changes.

If you get SQL errors "Can not create foreign key..." while execute generated script, it means referred table is not created yet. Just execute SQL script again and again until errors disappears ;(

Build
-----

./configure; make

For more detailed instruction please read INSTALL text file.


Windows
-------
Open proto-sql.vcxproj on VS2013, set include and library path to the protobuf location.

Dependencies
------------
protobuf

protoc


Usage
-----
Example given:

protoc --plugin=protoc-gen-sql="<path>/proto-sql" --proto_path=proto --sql_out=php  proto/fil-to-compile.proto 
