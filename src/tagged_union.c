
#include "./tagged_union.h"
#include "./json.h"

#include <tjson.h>
#include <tjson_helper.h>

TRTTI_DEFINE_TYPE_AS_SUPPORTED_EXTENDED(TaggedUnionArray*, TaggedUnionArrayPtr)

typedef struct {
	size_t struct_id;
} TaggedUnionProcessUserdata;

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnionProcessUserdata)

static JsonIterateResult json_value_iterator_process_tagged_unions(const JsonPath* path,
                                                                   RTTIAnnotatedValue parent,
                                                                   JsonIterateValue value,
                                                                   RTTIAnnotatedValue userdata) {

	UNUSED(userdata);

	if(json_path_is_root(path)) {

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArrayPtr, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT(
				        "implementation error, first RTTI value not 'TaggedUnionArrayPtr'") });
			}

			return new_json_iterate_result_ok(parent);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_END(value) {

			if(!TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArrayPtr, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_STATIC_LIT(
				        "implementation error, ARRAY END RTTI value not 'TaggedUnionArrayPtr'") });
			}

			// NOTE: here we could check some properties of the final result, alias if the type has
			// all fields set to a valid value

			// NOTE: return NON empty, as this is the toplevel parser, so the return value of this
			// is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		fprintf(stderr, "ERROR: type: " TRTTI_TYPE_NAME_FMT " tag: %u\n",
		        TRTTI_TYPE_NAME_FMT_ARGS(parent.type.name),
		        get_current_tag_type_for_json_iterate_value(value));

		return new_json_iterate_result_error((JsonIterateError){
		    .err = TSTR_STATIC_LIT("Error: unhandled iterate value in root path") });
	}

	return new_json_iterate_result_error((JsonIterateError){ .err = TSTR_STATIC_LIT("TODO") });
}

static void json_value_iterator_free_tagged_unions(RTTIAnnotatedValue data) {

	fprintf(stderr, "ERROR: don't know how to free: " TRTTI_TYPE_NAME_FMT "\n",
	        TRTTI_TYPE_NAME_FMT_ARGS(data.type.name));
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

	TaggedUnionProcessUserdata userdata = {
		.struct_id = 0,
	};

	RTTIAnnotatedValue userdata_value =
	    TRTTI_ANNOTATED_VALUE_GET(TaggedUnionProcessUserdata, &userdata);

	JsonIterateResult iterate_res =
	    json_value_iterate(&value, json_value_iterator_process_tagged_unions,
	                       json_value_iterator_free_tagged_unions, array_value, userdata_value);

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
