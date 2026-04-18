#include "./json.h"

#include <tjson_schema.h>

NODISCARD static JsonSchema get_json_schema(void) {

	JsonSchemaString* ParsedNamePascalCaseZ = json_schema_string_get();
	//.nonempty().regex(/^[A-Z][a-zA-Z0-9]*$/)

	JsonSchemaString* ParsedNameFromPartsZItems = json_schema_string_get();
	//.nonempty().regex(/^([A-Z0-9]*)|([a-z0-9]*)$/)

	JsonSchemaArray* ParsedNameFromPartsZ =
	    json_schema_array_get(new_json_schema_string(ParsedNameFromPartsZItems));
	// ).min(2)

	JsonSchemaOneOf* ParsedNameZ = json_schema_one_of_get_empty();

	{

		tstr_static add_result = json_schema_one_of_add_entry(
		    ParsedNameZ, new_json_schema_string(ParsedNamePascalCaseZ));
		assert(tstr_static_is_null(add_result));

		add_result =
		    json_schema_one_of_add_entry(ParsedNameZ, new_json_schema_array(ParsedNameFromPartsZ));
		assert(tstr_static_is_null(add_result));
	}

	JsonSchemaString* ParsedCTypeZ = json_schema_string_get();
	//.nonempty()

	JsonSchemaObject* ParsedTaggedTypeStructMemberZ = json_schema_object_get_empty();

	{

		tstr typeName = TSTR_LIT("typeName");

		tstr_static add_result = json_schema_object_add_entry(
		    ParsedTaggedTypeStructMemberZ, &typeName, new_json_schema_string(ParsedCTypeZ), true);
		assert(tstr_static_is_null(add_result));

		tstr name = TSTR_LIT("name");

		JsonSchemaString* NameSchema = json_schema_string_get();
		//.nonempty()

		add_result = json_schema_object_add_entry(ParsedTaggedTypeStructMemberZ, &name,
		                                          new_json_schema_string(NameSchema), true);
		assert(tstr_static_is_null(add_result));
	}

	/*

	const ParsedTaggedTypeStructZ = z.object({
	    struct: z.object({
	        members: z.array(ParsedTaggedTypeStructMemberZ).min(1)
	    }).strict()
	}).strict()



	const ParsedTaggedTypeSimpleZ = z.object({
	    name: z.string().nonempty()
	}).strict()


	const ParsedTaggedTypeZ = z.xor([ParsedTaggedTypeStructZ, ParsedTaggedTypeSimpleZ])

	const ParsedTaggedMemberZ = z.object({
	    name: ParsedNameZ,
	    type: z.xor([z.null(), ParsedTaggedTypeZ])
	}).strict()

	const ParsedCEnumTypeZ = z.xor([z.literal("bool"), z.literal("u8"), z.literal("u16"),
	z.literal("u32"), z.literal("u64")])

	const ParsedEnumZ = z.object({
	    name: ParsedNameZ,
	    underlyingType: z.xor([ParsedCEnumTypeZ, z.literal("best_match"), z.null()]).optional()
	}).strict()



	const StructOrderZ = z.xor([z.literal("auto"), z.literal("tag_first"), z.literal("tag_second")])

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
}

NODISCARD tstr generate_json_schema(void) {

	const JsonSchema schema = get_json_schema();

	json_schema_to_string(schema, (TODO){
	                                  unrepresentable = "throw",
	                                  cycles = "throw",
	                                  reused = "ref",
	                                  target = "draft-2020-12",
	                              });
}
