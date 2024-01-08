#include "Helpers.h"
#include "Object.h"
#include <string>

void moveOwnershipToVM(Variable& var)
{
	switch (var.getType())
	{
	case VariableType::Object: {
		// Currently all objects are char arrays

		// @todo: make string object
		String* str = new String(var.as<const char*>());
		var = reinterpret_cast<uint64_t>(str);

	} break;

	default:
		break;
	}

    return;
}

void moveOwnershipToHost(Variable& var)
{
	switch (var.getType())
	{
	case VariableType::Object: {
		if (var.as<Object*>()->getType() != ObjectType::String) return;
		auto str = var.as<String*>();
		char* out = new char[str->size()]();
		strcpy_s(out, str->size(), str->data());
		var = out;
	} break;

	default:
		break;
	}
	return;
}
