#include "proto-sql.h"

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
	// Don't print a silly message or stick a modal dialog box in my face, please.
	_set_abort_behavior(0, ~0);
#endif  // !_MSC_VER
	SQLCodeGenerator generator("protoc-gen-sql");
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
	return 0;
}

