#include "./json.h"

#include <tjson_schema.h>

NODISCARD JsonSchema get_json_schema_for_input(void) {

	JsonSchemaString* ParsedNamePascalCaseZ = json_schema_string_get();
	tstr_static add_result = json_schema_string_set_nonempty(ParsedNamePascalCaseZ);
	assert(tstr_static_is_null(add_result));

	JsonSchemaRegexResult ParsedNamePascalCaseZRegex = json_schema_regex_get("^[A-Z][a-zA-Z0-9]*$");
	assert(get_current_tag_type_for_json_schema_regex_result(ParsedNamePascalCaseZRegex) ==
	       JsonSchemaRegexResultTypeOk);

	add_result = json_schema_string_set_regex(
	    ParsedNamePascalCaseZ, json_schema_regex_result_get_as_ok(ParsedNamePascalCaseZRegex));
	assert(tstr_static_is_null(add_result));

	JsonSchemaString* ParsedNameFromPartsZItems = json_schema_string_get();
	add_result = json_schema_string_set_nonempty(ParsedNameFromPartsZItems);
	assert(tstr_static_is_null(add_result));

	JsonSchemaRegexResult ParsedNameFromPartsZItemsRegex =
	    json_schema_regex_get("^([A-Z0-9]*)|([a-z0-9]*)$");
	assert(get_current_tag_type_for_json_schema_regex_result(ParsedNamePascalCaseZRegex) ==
	       JsonSchemaRegexResultTypeOk);

	add_result = json_schema_string_set_regex(
	    ParsedNameFromPartsZItems,
	    json_schema_regex_result_get_as_ok(ParsedNameFromPartsZItemsRegex));
	assert(tstr_static_is_null(add_result));

	JsonSchemaArray* ParsedNameFromPartsZ =
	    json_schema_array_get(new_json_schema_string_rc(ParsedNameFromPartsZItems), true);
	assert(ParsedNameFromPartsZ != NULL);
	add_result = json_schema_array_set_min(ParsedNameFromPartsZ, 2);
	assert(tstr_static_is_null(add_result));

	JsonSchemaOneOf* ParsedNameZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(ParsedNameZ,
		                                          new_json_schema_string_rc(ParsedNamePascalCaseZ));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedNameZ,
		                                          new_json_schema_array_rc(ParsedNameFromPartsZ));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaString* ParsedCTypeZ = json_schema_string_get();
	add_result = json_schema_string_set_nonempty(ParsedCTypeZ);
	assert(tstr_static_is_null(add_result));

	JsonSchemaObject* ParsedTaggedTypeStructMemberZ = json_schema_object_get(false);

	{

		tstr typeName = TSTR_LIT("typeName");

		add_result = json_schema_object_add_entry_moved(
		    ParsedTaggedTypeStructMemberZ, TJSON_MOVE(tstr, typeName),
		    new_json_schema_string_rc(ParsedCTypeZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaString* NameSchema = json_schema_string_get();
		add_result = json_schema_string_set_nonempty(NameSchema);
		assert(tstr_static_is_null(add_result));

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry_moved(
		    ParsedTaggedTypeStructMemberZ, TJSON_MOVE(tstr, name),
		    new_json_schema_string_rc(NameSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedTypeStructZ = json_schema_object_get(false);

	{

		JsonSchemaObject* ParsedTaggedTypeStructZMemberObject = json_schema_object_get(false);

		{

			JsonSchemaArray* ParsedTaggedTypeStructZMembers = json_schema_array_get(
			    new_json_schema_object_rc(ParsedTaggedTypeStructMemberZ), true);
			assert(ParsedTaggedTypeStructZMembers != NULL);
			add_result = json_schema_array_set_min(ParsedTaggedTypeStructZMembers, 1);
			assert(tstr_static_is_null(add_result));

			tstr members = TSTR_LIT("members");

			add_result = json_schema_object_add_entry_moved(
			    ParsedTaggedTypeStructZMemberObject, TJSON_MOVE(tstr, members),
			    new_json_schema_array_rc(ParsedTaggedTypeStructZMembers), true);
			assert(tstr_static_is_null(add_result));
		}

		tstr struct_ = TSTR_LIT("struct");

		add_result = json_schema_object_add_entry_moved(
		    ParsedTaggedTypeStructZ, TJSON_MOVE(tstr, struct_),
		    new_json_schema_object_rc(ParsedTaggedTypeStructZMemberObject), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedTypeSimpleZ = json_schema_object_get(false);

	{

		JsonSchemaString* NameSchema = json_schema_string_get();
		add_result = json_schema_string_set_nonempty(NameSchema);
		assert(tstr_static_is_null(add_result));

		tstr name = TSTR_LIT("name");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedTypeSimpleZ, TJSON_MOVE(tstr, name),
		                                       new_json_schema_string_rc(NameSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* ParsedTaggedTypeZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(
		    ParsedTaggedTypeZ, new_json_schema_object_rc(ParsedTaggedTypeStructZ));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    ParsedTaggedTypeZ, new_json_schema_object_rc(ParsedTaggedTypeSimpleZ));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedMemberZ = json_schema_object_get(false);

	{

		tstr name = TSTR_LIT("name");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedMemberZ, TJSON_MOVE(tstr, name),
		                                       new_json_schema_one_of_rc(ParsedNameZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaOneOf* TypeSchema = json_schema_one_of_get_empty();

		{

			add_result = json_schema_one_of_add_entry(TypeSchema, new_json_schema_null());
			assert(tstr_static_is_null(add_result));

			add_result = json_schema_one_of_add_entry(TypeSchema,
			                                          new_json_schema_one_of_rc(ParsedTaggedTypeZ));
			assert(tstr_static_is_null(add_result));
		}

		tstr type_ = TSTR_LIT("type");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedMemberZ, TJSON_MOVE(tstr, type_),
		                                       new_json_schema_one_of_rc(TypeSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* ParsedCEnumTypeZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(
		    ParsedCEnumTypeZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("bool")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    ParsedCEnumTypeZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("u8")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    ParsedCEnumTypeZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("u16")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    ParsedCEnumTypeZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("u32")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    ParsedCEnumTypeZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("u64")));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedEnumZ = json_schema_object_get(false);

	{

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry_moved(
		    ParsedEnumZ, TJSON_MOVE(tstr, name), new_json_schema_one_of_rc(ParsedNameZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaOneOf* ParsedCEnumTypeZUnderlyingType = json_schema_one_of_get_empty();

		{

			add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZUnderlyingType,
			                                          new_json_schema_one_of_rc(ParsedCEnumTypeZ));
			assert(tstr_static_is_null(add_result));

			add_result = json_schema_one_of_add_entry(
			    ParsedCEnumTypeZUnderlyingType,
			    new_json_schema_literal_rc(json_schema_literal_get_cstr("best_match")));
			assert(tstr_static_is_null(add_result));

			add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZUnderlyingType,
			                                          new_json_schema_null());
			assert(tstr_static_is_null(add_result));
		}

		tstr underlyingType = TSTR_LIT("underlyingType");

		add_result = json_schema_object_add_entry_moved(
		    ParsedEnumZ, TJSON_MOVE(tstr, underlyingType),
		    new_json_schema_one_of_rc(ParsedCEnumTypeZUnderlyingType), false);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* StructOrderZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(
		    StructOrderZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("auto")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    StructOrderZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("tag_first")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    StructOrderZ, new_json_schema_literal_rc(json_schema_literal_get_cstr("tag_second")));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* StructOrderRequirementZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(
		    StructOrderRequirementZ,
		    new_json_schema_literal_rc(json_schema_literal_get_cstr("best_size")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(
		    StructOrderRequirementZ,
		    new_json_schema_literal_rc(json_schema_literal_get_cstr("aligned_access")));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedUnionRequirementsZ = json_schema_object_get(false);

	{

		tstr order = TSTR_LIT("order");

		add_result = json_schema_object_add_entry_moved(
		    ParsedTaggedUnionRequirementsZ, TJSON_MOVE(tstr, order),
		    new_json_schema_one_of_rc(StructOrderRequirementZ), false);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedCppFeaturesZ = json_schema_object_get(false);

	{

		tstr order = TSTR_LIT("tagAsErrorVariant");

		add_result = json_schema_object_add_entry_moved(ParsedCppFeaturesZ, TJSON_MOVE(tstr, order),
		                                                new_json_schema_boolean(), false);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedOptionsZ = json_schema_object_get(false);

	{

		tstr rawStruct = TSTR_LIT("rawStruct");

		add_result =
		    json_schema_object_add_entry_moved(ParsedOptionsZ, TJSON_MOVE(tstr, rawStruct),
		                                       new_json_schema_one_of_rc(ParsedNameZ), false);
		assert(tstr_static_is_null(add_result));

		tstr structOrder = TSTR_LIT("structOrder");

		add_result =
		    json_schema_object_add_entry_moved(ParsedOptionsZ, TJSON_MOVE(tstr, structOrder),
		                                       new_json_schema_one_of_rc(StructOrderZ), false);
		assert(tstr_static_is_null(add_result));

		tstr requirements = TSTR_LIT("requirements");

		add_result = json_schema_object_add_entry_moved(
		    ParsedOptionsZ, TJSON_MOVE(tstr, requirements),
		    new_json_schema_object_rc(ParsedTaggedUnionRequirementsZ), false);
		assert(tstr_static_is_null(add_result));

		tstr cppFeatures = TSTR_LIT("cppFeatures");

		add_result = json_schema_object_add_entry_moved(
		    ParsedOptionsZ, TJSON_MOVE(tstr, cppFeatures),
		    new_json_schema_object_rc(ParsedCppFeaturesZ), false);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedUnionZ = json_schema_object_get(false);

	{

		tstr name = TSTR_LIT("name");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedUnionZ, TJSON_MOVE(tstr, name),
		                                       new_json_schema_one_of_rc(ParsedNameZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaArray* ParsedTaggedUnionZMember =
		    json_schema_array_get(new_json_schema_object_rc(ParsedTaggedMemberZ), true);
		assert(ParsedTaggedUnionZMember != NULL);
		add_result = json_schema_array_set_min(ParsedTaggedUnionZMember, 2);
		assert(tstr_static_is_null(add_result));

		tstr member = TSTR_LIT("member");

		add_result = json_schema_object_add_entry_moved(
		    ParsedTaggedUnionZ, TJSON_MOVE(tstr, member),
		    new_json_schema_array_rc(ParsedTaggedUnionZMember), true);
		assert(tstr_static_is_null(add_result));

		tstr enum_ = TSTR_LIT("enum");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedUnionZ, TJSON_MOVE(tstr, enum_),
		                                       new_json_schema_object_rc(ParsedEnumZ), true);
		assert(tstr_static_is_null(add_result));

		tstr options = TSTR_LIT("options");

		add_result =
		    json_schema_object_add_entry_moved(ParsedTaggedUnionZ, TJSON_MOVE(tstr, options),
		                                       new_json_schema_object_rc(ParsedOptionsZ), false);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaArray* ParsedTaggedUnionFullSchemaZ =
	    json_schema_array_get(new_json_schema_object_rc(ParsedTaggedUnionZ), true);
	assert(ParsedTaggedUnionFullSchemaZ != NULL);
	add_result = json_schema_array_set_min(ParsedTaggedUnionFullSchemaZ, 1);
	assert(tstr_static_is_null(add_result));

	return new_json_schema_array_rc(ParsedTaggedUnionFullSchemaZ);
}
