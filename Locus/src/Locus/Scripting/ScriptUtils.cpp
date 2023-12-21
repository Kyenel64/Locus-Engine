#include "Lpch.h"
#include "ScriptUtils.h"

namespace Locus
{
	namespace ScriptUtils
	{
		char* ReadBytes(const std::string& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = (uint32_t)(end - stream.tellg());

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		// Converts MonoType to Locus's FieldType.
		FieldType MonoTypeToFieldType(MonoType* monoType)
		{
			std::string typeName = std::string(mono_type_get_name(monoType));
			if (typeName == "System.Single")  return FieldType::SystemSingle;
			if (typeName == "System.Double")  return FieldType::SystemDouble;
			if (typeName == "System.Int16")   return FieldType::SystemShort;
			if (typeName == "System.Int32")   return FieldType::SystemInt;
			if (typeName == "System.Int64")   return FieldType::SystemLong;
			if (typeName == "System.UInt16")  return FieldType::SystemUShort;
			if (typeName == "System.UInt32")  return FieldType::SystemUInt;
			if (typeName == "System.UInt64")  return FieldType::SystemULong;
			if (typeName == "System.Boolean") return FieldType::SystemBoolean;
			if (typeName == "System.Char")    return FieldType::SystemChar;

			if (typeName == "Locus.Vec2")     return FieldType::LocusVec2;
			if (typeName == "Locus.Vec3")     return FieldType::LocusVec3;
			if (typeName == "Locus.Vec4")     return FieldType::LocusVec4;
			if (typeName == "Locus.Entity")   return FieldType::LocusEntity;
			return FieldType::None;
		}

		bool CheckMonoError(MonoError& error)
		{
			bool hasError = !mono_error_ok(&error);

			if (hasError)
			{
				unsigned short errorCode = mono_error_get_error_code(&error);
				const char* errorMessage = mono_error_get_message(&error);

				LOCUS_SCRIPT_ERROR("Mono Error!");
				LOCUS_SCRIPT_ERROR("\tError Code: {0}", errorCode);
				LOCUS_SCRIPT_ERROR("\tError Message: {0}", errorMessage);
				mono_error_cleanup(&error);
				LOCUS_CORE_ASSERT(false, "Error");
			}

			return hasError;
		}

		std::string MonoStringToUTF8(MonoString* monoString)
		{
			if (monoString == nullptr || mono_string_length(monoString) == 0)
				return "";

			MonoError error;
			char* utf8 = mono_string_to_utf8_checked(monoString, &error);
			if (CheckMonoError(error))
				return "";
			std::string result(utf8);
			mono_free(utf8);
			return result;
		}

		void ProcessException(MonoException* exception)
		{
			if (exception)
			{
				//mono_print_unhandled_exception((MonoObject*)exception);
				MonoClass* exceptionClass = mono_object_get_class((MonoObject*)exception);
				MonoType* exceptionType = mono_class_get_type(exceptionClass);

				auto GetExceptionString = [exception, exceptionClass](const char* stringName) -> std::string
					{
						MonoProperty* prop = mono_class_get_property_from_name(exceptionClass, stringName);

						if (prop == nullptr)
							return "";

						MonoMethod* getterMethod = mono_property_get_get_method(prop);

						if (getterMethod == nullptr)
							return "";

						MonoString* string = (MonoString*)mono_runtime_invoke(getterMethod, exception, nullptr, nullptr);
						return MonoStringToUTF8(string);
					};

				ExceptionData exceptionData;
				exceptionData.Type = mono_type_get_name(exceptionType);
				//exceptionData.Source = GetExceptionString("Source");
				exceptionData.Message = GetExceptionString("Message");
				exceptionData.Trace = GetExceptionString("StackTrace");

				ScriptEngine::GetExceptions().push(exceptionData);
			}
		}
	}
}