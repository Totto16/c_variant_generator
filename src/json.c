#include "./json.h"

#include <tjson_schema.h>

NODISCARD static JsonSchema get_json_schema(void) {

	JsonSchemaString* ParsedNamePascalCaseZ = json_schema_string_get();
	tstr_static add_result = json_schema_string_set_nonempty(ParsedNamePascalCaseZ);
	assert(tstr_static_is_null(add_result));
	add_result = json_schema_string_set_regex(ParsedNamePascalCaseZ,
	                                          json_schema_regex_get("/^[A-Z][a-zA-Z0-9]*$/"));
	assert(tstr_static_is_null(add_result));

	JsonSchemaString* ParsedNameFromPartsZItems = json_schema_string_get();
	add_result = json_schema_string_set_nonempty(ParsedNameFromPartsZItems);
	assert(tstr_static_is_null(add_result));
	add_result = json_schema_string_set_regex(ParsedNameFromPartsZItems,
	                                          json_schema_regex_get("/^([A-Z0-9]*)|([a-z0-9]*)$/"));
	assert(tstr_static_is_null(add_result));

	JsonSchemaArray* ParsedNameFromPartsZ =
	    json_schema_array_get(new_json_schema_string(ParsedNameFromPartsZItems), true);
	assert(ParsedNameFromPartsZ != NULL);
	add_result = json_schema_array_set_min(ParsedNameFromPartsZ, 2);
	assert(tstr_static_is_null(add_result));

	JsonSchemaOneOf* ParsedNameZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(ParsedNameZ,
		                                          new_json_schema_string(ParsedNamePascalCaseZ));
		assert(tstr_static_is_null(add_result));

		add_result =
		    json_schema_one_of_add_entry(ParsedNameZ, new_json_schema_array(ParsedNameFromPartsZ));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaString* ParsedCTypeZ = json_schema_string_get();
	add_result = json_schema_string_set_nonempty(ParsedCTypeZ);
	assert(tstr_static_is_null(add_result));

	JsonSchemaObject* ParsedTaggedTypeStructMemberZ = json_schema_object_get(false);

	{

		tstr typeName = TSTR_LIT("typeName");

		add_result = json_schema_object_add_entry(ParsedTaggedTypeStructMemberZ, &typeName,
		                                          new_json_schema_string(ParsedCTypeZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaString* NameSchema = json_schema_string_get();
		add_result = json_schema_string_set_nonempty(NameSchema);
		assert(tstr_static_is_null(add_result));

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry(ParsedTaggedTypeStructMemberZ, &name,
		                                          new_json_schema_string(NameSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedTypeStructZ = json_schema_object_get(false);

	{

		JsonSchemaObject* ParsedTaggedTypeStructZMemberObject = json_schema_object_get(false);

		{

			JsonSchemaArray* ParsedTaggedTypeStructZMembers =
			    json_schema_array_get(new_json_schema_object(ParsedTaggedTypeStructMemberZ), true);
			assert(ParsedTaggedTypeStructZMembers != NULL);
			add_result = json_schema_array_set_min(ParsedTaggedTypeStructZMembers, 1);
			assert(tstr_static_is_null(add_result));

			tstr members = TSTR_LIT("members");

			add_result = json_schema_object_add_entry(
			    ParsedTaggedTypeStructZMemberObject, &members,
			    new_json_schema_array(ParsedTaggedTypeStructZMembers), true);
			assert(tstr_static_is_null(add_result));
		}

		tstr struct_ = TSTR_LIT("struct");

		add_result = json_schema_object_add_entry(
		    ParsedTaggedTypeStructZ, &struct_,
		    new_json_schema_object(ParsedTaggedTypeStructZMemberObject), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedTypeSimpleZ = json_schema_object_get(false);

	{

		JsonSchemaString* NameSchema = json_schema_string_get();
		add_result = json_schema_string_set_nonempty(NameSchema);
		assert(tstr_static_is_null(add_result));

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry(ParsedTaggedTypeSimpleZ, &name,
		                                          new_json_schema_string(NameSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* ParsedTaggedTypeZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(ParsedTaggedTypeZ,
		                                          new_json_schema_object(ParsedTaggedTypeStructZ));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedTaggedTypeZ,
		                                          new_json_schema_object(ParsedTaggedTypeSimpleZ));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedTaggedMemberZ = json_schema_object_get(false);

	{

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry(ParsedTaggedMemberZ, &name,
		                                          new_json_schema_one_of(ParsedNameZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaOneOf* TypeSchema = json_schema_one_of_get_empty();

		{

			add_result = json_schema_one_of_add_entry(TypeSchema, new_json_schema_null());
			assert(tstr_static_is_null(add_result));

			add_result =
			    json_schema_one_of_add_entry(TypeSchema, new_json_schema_one_of(ParsedTaggedTypeZ));
			assert(tstr_static_is_null(add_result));
		}

		tstr type_ = TSTR_LIT("type");

		add_result = json_schema_object_add_entry(ParsedTaggedMemberZ, &type_,
		                                          new_json_schema_one_of(TypeSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaOneOf* ParsedCEnumTypeZ = json_schema_one_of_get_empty();

	{

		add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZ,
		                                          new_json_schema_literal(JSON_SCHEMA_LIT("bool")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZ,
		                                          new_json_schema_literal(JSON_SCHEMA_LIT("u8")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZ,
		                                          new_json_schema_literal(JSON_SCHEMA_LIT("u16")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZ,
		                                          new_json_schema_literal(JSON_SCHEMA_LIT("u32")));
		assert(tstr_static_is_null(add_result));

		add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZ,
		                                          new_json_schema_literal(JSON_SCHEMA_LIT("u64")));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaObject* ParsedEnumZ = json_schema_object_get(false);

	{

		tstr name = TSTR_LIT("name");

		add_result = json_schema_object_add_entry(ParsedEnumZ, &name,
		                                          new_json_schema_one_of(ParsedNameZ), true);
		assert(tstr_static_is_null(add_result));

		JsonSchemaOneOf* ParsedCEnumTypeZUnderlyingType = json_schema_one_of_get_empty();

		{

			add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZUnderlyingType,
			                                          new_json_schema_one_of(ParsedCEnumTypeZ));
			assert(tstr_static_is_null(add_result));

			add_result = json_schema_one_of_add_entry(
			    ParsedCEnumTypeZUnderlyingType,
			    new_json_schema_literal(JSON_SCHEMA_LIT("best_match")));
			assert(tstr_static_is_null(add_result));

			add_result = json_schema_one_of_add_entry(ParsedCEnumTypeZUnderlyingType,
			                                          new_json_schema_null());
			assert(tstr_static_is_null(add_result));
		}

		tstr underlyingType = TSTR_LIT("underlyingType");

		add_result = json_schema_object_add_entry(
		    ParsedEnumZ, &underlyingType, new_json_schema_one_of(ParsedCEnumTypeZUnderlyingType),
		    false);
		assert(tstr_static_is_null(add_result));
	}

	/*
	const StructOrderZ = z.xor([z.literal("auto"), z.literal("tag_first"),
	z.literal("tag_second")])

	const StructOrderRequirementZ = z.xor([z.literal("best_size"), z.literal("aligned_access")])



	const ParsedTaggedUnionRequirementsZ = z.object(
	    {
	        order: StructOrderRequirementZ.optional()
	    }
	).strict()



	const ParsedCppFeaturesZ = z.object(
	    {
	        tagAsErrorVariant: z.boolean().optional()
	    }
	).strict()


	const ParsedOptionsZ = z.object({
	    rawStruct: ParsedNameZ.optional(),
	    structOrder: StructOrderZ.optional(),
	    requirements: ParsedTaggedUnionRequirementsZ.optional(),
	    cppFeatures: ParsedCppFeaturesZ.optional(),
	}).strict()


	    const auto ParsedTaggedUnionZ = z.object({
	                                         name : ParsedNameZ,
	                                         member : z.array(ParsedTaggedMemberZ).min(2),
	                                         enum : ParsedEnumZ,
	                                         options : z.optional(ParsedOptionsZ).optional()
	                                     })
	                                        .strict();

	    const auto ParsedTaggedUnionFullSchemaZ = z.array(ParsedTaggedUnionZ).min(1); */

	return ParsedTaggedUnionFullSchemaZ;
}

NODISCARD tstr generate_json_schema(void) {

	JsonSchema schema = get_json_schema();

	const tstr result = json_schema_to_string(&schema);

	free_json_schema(&schema);

	if(tstr_is_null(&result)) {
		return tstr_null();
	}

	return result;
}
