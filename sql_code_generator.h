#ifndef SQL_CODE_GENERATOR_H
#define SQL_CODE_GENERATOR_H	1

#include <string>
#include <map>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>

typedef std::vector<const google::protobuf::Descriptor*> messagetypes;

class SQLCodeGenerator : public google::protobuf::compiler::CodeGenerator {
	public:
		std::map<std::string, std::string> sqltypes;
		SQLCodeGenerator(const std::string& name);
		virtual ~SQLCodeGenerator();
		virtual bool Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter, google::protobuf::compiler::GeneratorContext* context, std::string* error) const;
	private:
		static void listOne2Many(const google::protobuf::FileDescriptor* file, std::map<const google::protobuf::Descriptor*, messagetypes*> *repeatedmessages);
};

#endif