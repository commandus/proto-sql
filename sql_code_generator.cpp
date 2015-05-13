#include "sql_code_generator.h"

#include <iostream>
#include <sstream>
#include <algorithm>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/stl_util.h>

SQLCodeGenerator::SQLCodeGenerator(const std::string& name)
{
	sqltypes["int32"] = "int(11)";
	sqltypes["int64"] = "int(20)";
	sqltypes["uint32"] = "int(11)";
	sqltypes["uint64"] = "int(20)";
	sqltypes["double"] = "double";
	sqltypes["float"] = "float";
	sqltypes["bool"] = "tinyint(1)";
	sqltypes["enum"] = "enum";
	sqltypes["string"] = "text";
	sqltypes["message"] = "";
}

SQLCodeGenerator::~SQLCodeGenerator() 
{
}

void SQLCodeGenerator::listOne2Many(const google::protobuf::FileDescriptor* file, std::map<const google::protobuf::Descriptor*, messagetypes*> *repeatedmessages)
{
	// create for all messages list of used repeated messages
	for (int i = 0; i < file->message_type_count(); i++)
	{
		messagetypes *mt = new messagetypes();
		repeatedmessages->insert(std::pair<const google::protobuf::Descriptor*, messagetypes*>(file->message_type(i), mt));
	}

	// search for repeated only
	for (int i = 0; i < file->message_type_count(); i++)
	{
		const google::protobuf::Descriptor *m = file->message_type(i);
		for (int f = 0; f < m->field_count(); f++)
		{
			const google::protobuf::FieldDescriptor *fd = m->field(f);
			if ((!fd->is_repeated()) || (fd->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE))
				continue;

			const google::protobuf::Descriptor *mt = fd->message_type();
			messagetypes *ls = repeatedmessages->find(mt)->second;
			ls->push_back(m);
		}
	}
}

std::string getSuffix(const google::protobuf::FieldDescriptor *fd)
{
	std::stringstream ss;
	if (fd->is_required())
		ss << " NOT NULL ";
	if (fd->has_default_value())
	{
		ss << " DEFAULT ";
		switch (fd->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			ss << fd->default_value_int32();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			ss << fd->default_value_int64();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			ss << fd->default_value_uint32();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			ss << fd->default_value_uint64();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			ss << fd->default_value_double();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			ss << fd->default_value_float();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			ss << fd->default_value_bool();
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		{
			ss << "'" << fd->default_value_enum()->number() << "'";
		}
		break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			ss << "'" << fd->default_value_string() << "'";
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			ss << "0";
			break;
		default:
			ss << "0";
		}
	}
	return ss.str();
}

bool SQLCodeGenerator::Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter, google::protobuf::compiler::GeneratorContext* context, std::string* error)  const
{
	google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> output(context->Open(file->name() + ".sql"));
	google::protobuf::io::Printer printer(output.get(), '$');

	std::map<const google::protobuf::Descriptor*, messagetypes*> repeatedmessages;
	listOne2Many(file, &repeatedmessages);

	std::stringstream ss;
	ss << "SET charset 'utf8';" << std::endl;
	
	ss << "/*" << std::endl;
	for (int i = 0; i < file->message_type_count(); i++)
	{
		const google::protobuf::Descriptor *m = file->message_type(i);
		ss << "DROP TABLE IF EXISTS " << m->name() << ";" << std::endl;
	}
	ss << "*/" << std::endl;
	printer.PrintRaw(ss.str());

	for (int i = 0; i < file->message_type_count(); i++) 
	{
		const google::protobuf::Descriptor *m = file->message_type(i);
		std::stringstream ss;
		ss << "CREATE TABLE IF NOT EXISTS `" << m->name() << "`(" << std::endl;
		ss << "  `id` int(20) NOT NULL primary key AUTO_INCREMENT," << std::endl;


		// foreign keys
		messagetypes *fk = repeatedmessages.at(file->message_type(i));
		for (messagetypes::iterator it(fk->begin()); it != fk->end(); ++it)
		{
			const google::protobuf::Descriptor *d = *it;
			std::string lcn(d->name());
			std::transform(lcn.begin(), lcn.end(), lcn.begin(), ::tolower);
			std::string lcmn(file->message_type(i)->name());
			std::transform(lcmn.begin(), lcmn.end(), lcmn.begin(), ::tolower);

			ss << "  `" << lcn << "id` int(20) NOT NULL," << std::endl;
			ss << "  KEY `key_" << lcmn << "_" << lcn << "id` (`" << lcn << "id`)," << std::endl;
		}

		for (int f = 0; f < m->field_count(); f++)
		{
			const google::protobuf::FieldDescriptor *fd = m->field(f);

			google::protobuf::FieldDescriptor::CppType ct = fd->cpp_type();
			switch (ct)
			{
				case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
					if (!fd->is_repeated())
					{
						ss << "  `" << fd->lowercase_name() << "id` int(20)" << getSuffix(fd) << ", " << std::endl;
						std::string lcmn(m->name());
						std::transform(lcmn.begin(), lcmn.end(), lcmn.begin(), ::tolower);
						ss << "  KEY `" << "key_" << lcmn << "_" << fd->lowercase_name() << "id` (`" << fd->lowercase_name() << "id`), " << std::endl;
						ss << "  CONSTRAINT `fk_" << lcmn << "_" << fd->lowercase_name() << "id` FOREIGN KEY(`" << fd->lowercase_name() << "id`) REFERENCES `" 
							<< fd->message_type()->name() << "`(`" << "id" << "`) ON DELETE CASCADE," << std::endl;
					}
					break;
				case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
					{
						ss << "  `" << fd->lowercase_name() << "` ";
						ss << sqltypes.at(fd->cpp_type_name()) << "(";
						const google::protobuf::EnumDescriptor *ed = fd->enum_type();
						for (int ei = 0; ei < ed->value_count(); ei++)
						{
							const google::protobuf::EnumValueDescriptor *ev = ed->value(ei);
							ss << "'" << ev->number() << "'";
							if (ei < ed->value_count() - 1)
								ss << ", ";
						}
						ss << ")" << getSuffix(fd) << ", " << std::endl;
					}
					break;
				default:
					ss << "  `" << fd->lowercase_name() << "` " << sqltypes.at(fd->cpp_type_name()) << getSuffix(fd) << ", " << std::endl;
			}
		}
		ss << "  `tag` int(11)" << std::endl;
		ss << ");" << std::endl << std::endl;
		printer.PrintRaw(ss.str());
	}

	if (printer.failed()) 
	{
		*error = "SQLCodeGenerator detected write error.";
		return false;
	}
	return true;
}

