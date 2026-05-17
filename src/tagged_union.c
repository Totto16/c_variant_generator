
#include "./tagged_union.h"
#include "./json.h"

#include <tjson.h>
#include <tjson_helper.h>

TRTTI_DEFINE_TYPE_AS_SUPPORTED_EXTENDED(TaggedUnionArray*, TaggedUnionArrayPtr)

static JsonIterateResult json_value_iterator_tagged_unions(const JsonPath* path,
                                                           RTTIAnnotatedValue parent,
                                                           JsonIterateValue value) {
	// TODO
	UNUSED(path);
	UNUSED(parent);
	UNUSED(value);

	return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_STATIC_LIT("TODO") });
}

NODISCARD tstr_static get_tagged_unions(const tstr* const input_file, TaggedUnionArray* array) {

	JsonParseResult parse_result = json_value_parse_from_file(input_file);

	IF_JSON_PARSE_RESULT_IS_ERROR_CONST(parse_result) {

		tstr error_str = json_format_error(error);

		if(tstr_is_null(&error_str)) {
			return TSTR_STATIC_LIT("file parse error");
		}

		fprintf(stderr, "File parse error: " TSTR_FMT "\n", TSTR_FMT_ARGS(error_str));
		tstr_free(&error_str);
		return TSTR_STATIC_LIT("file parse error");
	}

	JsonValue value = json_parse_result_get_as_ok(parse_result);

#define FREE_AT_END() \
	do { \
		free_json_value(&value); \
	} while(false)

	JsonSchema schema = get_json_schema_for_input();

	tstr json_schema_error = json_schema_validate_data(&schema, &value);

	free_json_schema(&schema);

	if(!tstr_is_null(&json_schema_error)) {

		fprintf(stderr, "Schema error: " TSTR_FMT "\n", TSTR_FMT_ARGS(json_schema_error));
		tstr_free(&json_schema_error);
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file not correct schema");
	}

	FREE_AT_END();
	*array = TVEC_EMPTY(TaggedUnion);

	RTTIAnnotatedValue array_value = TRTTI_ANNOTATED_VALUE_GET(TaggedUnionArrayPtr, &array);

	JsonIterateResult iterate_res =
	    json_value_iterate(&value, json_value_iterator_tagged_unions, array_value);

	IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(iterate_res) {

		fprintf(stderr, "Json data get error: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(error.error.err));
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file getting after schema check failed");
	}

	return TSTR_STATIC_LIT("TODO");
}

#undef FREE_AT_END

void free_tagged_union_array(TaggedUnionArray* array) {
	// TODO
	UNUSED(array);
}
