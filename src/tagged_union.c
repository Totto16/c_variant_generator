
#include "./tagged_union.h"
#include "./colored_fprintf.h"
#include "./json.h"

#include <tjson.h>
#include <tjson_helper.h>

#include <fcntl.h>
#include <unistd.h>

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnionArray)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnion)

TVEC_IMPLEMENT_VEC_TYPE(TaggedUnion)

typedef struct {
	size_t struct_id;
} TaggedUnionProcessUserdata;

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnionProcessUserdata)

static JsonIterateResult json_value_iterator_process_tagged_unions(const JsonPath* path,
                                                                   RTTIAnnotatedValue parent,
                                                                   JsonIterateValue value,
                                                                   RTTIAnnotatedValue userdata) {

	TaggedUnionProcessUserdata* union_userdata =
	    TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionProcessUserdata, userdata);

	UNUSED(union_userdata);

	if(json_path_is_root(path)) {

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {
			// we already "allocated" (initialized the array), nothing to do here

			if(!TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err =
				        TSTR_LIT("implementation error, first RTTI value not 'TaggedUnionArray'"),
				    .path = NULL,
				});
			}

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    NULL, "Error: unhandled iterate value root path: <no more info>",
		    "Error: unhandled iterate value in root path: " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, parent)) {

		TaggedUnionArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionArray, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			TaggedUnion* union_slot = TVEC_PUSH_SLOT(TaggedUnion, array);

			if(union_slot == NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*union_slot = TAGGED_UNION_NULL();

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TaggedUnion, union_slot);

			return new_json_iterate_result_ok(result);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    NULL, "Error: unhandled iterate value at 'TaggedUnionArray': <no more info>",
		    "Error: unhandled iterate value at 'TaggedUnionArray': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

	RETURN_JSON_ITERATE_ERROR_DYNAMIC(
	    NULL, "Error: unhandled iterate value: <no more info>",
	    "Error: unhandled iterate value: " TSTR_FMT " on rtti: " TRTTI_TYPE_NAME_FMT,
	    TSTR_STATIC_FMT_ARGS(tag_name), TRTTI_TYPE_NAME_FMT_ARGS(parent.type.name));
}

static void json_value_iterator_free_tagged_unions(RTTIAnnotatedValue data) {

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, data)) {

		TaggedUnionArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionArray, data);

		free_tagged_union_array(array);
		// no free of the pointer as it is a address to the stack of the parent function

		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnion, data)) {

		TaggedUnion* tagged_union = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnion, data);

		free_tagged_union(tagged_union);
		// no free of the pointer as it is a push slot of the array
		return;
	}

	color_fprintf(stderr,
	              COLOR_RED "ERROR: don't know how to free: " COLOR_YELLOW TRTTI_TYPE_NAME_FMT
	                        "\n" COLOR_RESET,
	              TRTTI_TYPE_NAME_FMT_ARGS(data.type.name));
}

NODISCARD static bool file_exists(const tstr* const input_file) {

	int fd = open(tstr_cstr(input_file), O_RDONLY);

	if(fd < 0) {
		errno = 0;
		return false;
	}

	close(fd);
	return true;
}

NODISCARD tstr_static get_tagged_unions(const tstr* const input_file, TaggedUnionArray* array) {

	if(!file_exists(input_file)) {
		color_fprintf(stderr,
		              COLOR_RED "File does not exist: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(*input_file));
		return TSTR_STATIC_LIT("file does not exist");
	}

	JsonParseResult parse_result = json_value_parse_from_file(input_file);

	IF_JSON_PARSE_RESULT_IS_ERROR_CONST(parse_result) {

		tstr error_str = json_format_error(error);

		if(tstr_is_null(&error_str)) {
			color_fprintf(stderr, COLOR_RED "File parse error: <unknown>\n" COLOR_RESET);
			return TSTR_STATIC_LIT("file parse error");
		}

		color_fprintf(stderr, COLOR_RED "File parse error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(error_str));
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

		color_fprintf(stderr, COLOR_RED "Schema error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(json_schema_error));
		tstr_free(&json_schema_error);
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file not correct schema");
	}

	*array = TVEC_EMPTY(TaggedUnion);

	RTTIAnnotatedValue array_value = TRTTI_ANNOTATED_VALUE_GET(TaggedUnionArray, array);

	TaggedUnionProcessUserdata userdata = {
		.struct_id = 0,
	};

	RTTIAnnotatedValue userdata_value =
	    TRTTI_ANNOTATED_VALUE_GET(TaggedUnionProcessUserdata, &userdata);

	JsonIterateResult iterate_res =
	    json_value_iterate(&value, json_value_iterator_process_tagged_unions,
	                       json_value_iterator_free_tagged_unions, array_value, userdata_value);

	IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF(&iterate_res) {

		auto err_str = json_iterate_error_to_str(&(error->error));

		if(tstr_is_null(&err_str)) {
			color_fprintf(stderr,
			              COLOR_RED "Json data get error: " COLOR_YELLOW "<unknown>\n" COLOR_RESET);

			free_json_iterate_error(&(error->error));
			FREE_AT_END();
			return TSTR_STATIC_LIT("input file getting after schema check failed");
		}

		color_fprintf(stderr,
		              COLOR_RED "Json data get error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(err_str));

		tstr_free(&err_str);
		free_json_iterate_error(&(error->error));
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file getting after schema check failed");
	}

	free_json_value(&value);

	return TSTR_STATIC_LIT("TODO");
}

#undef FREE_AT_END

void free_tagged_union(TaggedUnion* tagged_union) {
	// TODO
	UNUSED(tagged_union);
}

void free_tagged_union_array(TaggedUnionArray* array) {

	for(size_t i = 0; i < TVEC_LENGTH(TaggedUnion, *array); ++i) {
		TaggedUnion* value = TVEC_GET_AT_MUT(TaggedUnion, array, i);

		free_tagged_union(value);
	}

	TVEC_FREE(TaggedUnion, array);
}
