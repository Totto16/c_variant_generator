
#include "./generate.h"

#include <cwalk.h>

NODISCARD static tstr_view cwk_wrapper_get_ext(tstr_static value) {

	const char* ext = NULL;
	size_t len = 0;
	bool result = cwk_path_get_extension(value.ptr, &ext, &len);

	if(!result) {
		return TSTR_EMPTY_VIEW;
	}

	return (tstr_view){ .data = ext, .len = len };
}

NODISCARD ExitCode generate_variants(tstr_static input, tstr_static output) {

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

	// TODO
	fprintf(stderr, "TODO\n");

	return ExitCodeFailure;
}
