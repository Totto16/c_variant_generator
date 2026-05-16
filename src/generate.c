
#include "./generate.h"

#include <cwalk.h>

#include <tstr_builder.h>

NODISCARD static tstr_view cwk_wrapper_get_ext(tstr_static value) {

	const char* ext = NULL;
	size_t len = 0;
	bool result = cwk_path_get_extension(value.ptr, &ext, &len);

	if(!result) {
		return TSTR_EMPTY_VIEW;
	}

	return (tstr_view){ .data = ext, .len = len };
}

NODISCARD static bool write_file_helper(tstr_static file, const tstr* value) {

	// TODO
	UNUSED(file);
	UNUSED(value);

	return false;
}

NODISCARD ExitCode generate_variants(tstr_static input, tstr_static output) {

	if(input.ptr == NULL || output.ptr == NULL) {
		fprintf(stderr, "IMPLEMENTATION ERROR: input strings are <NULL>\n");
		return ExitCodeFailure;
	}

	if(!cwk_path_is_absolute(input.ptr)) {
		fprintf(stderr, "Input file path is not absolute:" TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		return ExitCodeFailure;
	}

	if(!cwk_path_is_absolute(output.ptr)) {
		fprintf(stderr, "Output file path is not absolute:" TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		return ExitCodeFailure;
	}

	const tstr_view output_extension = cwk_wrapper_get_ext(output);

	if(!tstr_view_eq_view(output_extension, TSTR_TSV(".h"))) {
		fprintf(stderr, "Output file has not a .h extension:" TSTR_FMT "\n",
		        TSV_FMT_ARGS(output_extension));
		return ExitCodeFailure;
	}

	StringBuilder* string_builder = string_builder_init();

	if(string_builder == NULL) {
		return ExitCodeFailure;
	}

#define FREE_AT_END() \
	do { \
		free_string_builder(string_builder); \
	} while(false)

	tstr value = string_builder_release_into_tstr(&string_builder);

	if(tstr_is_null(&value)) {
		FREE_AT_END();
		fprintf(stderr, "Error: couldn't get string from string builder\n");
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&value); \
		free_string_builder(string_builder); \
	} while(false)

	bool write_result = write_file_helper(output, &value);

	if(!write_result) {
		FREE_AT_END();
		fprintf(stderr, "Error: couldn't write to file: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		return ExitCodeFailure;
	}

	FREE_AT_END();

	return ExitCodeSuccess;
}

#undef FREE_AT_END
