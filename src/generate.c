
#include "./generate.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

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

NODISCARD static int make_dir_recursive(char* dir) {
	if(dir == NULL) {
		return -1;
	}

	if(*dir == '\0') {
		return 0;
	}

	for(char* p = strchr(dir + 1, '/'); p != NULL; p = strchr(p + 1, '/')) {
		*p = '\0';

		int mkdir_result = mkdir(dir, S_IRWXU);

		if(mkdir_result == -1) {
			if(errno != EEXIST) {
				*p = '/';
				return -errno;
			}
		}

		*p = '/';
	}

	return 0;
}

NODISCARD static bool write_file_helper(tstr_static file, const tstr* value) {

	size_t dirname_length = 0;

	cwk_path_get_dirname(file.ptr, &dirname_length);

	if(dirname_length != 0) {

		tstr_view dirname = { .data = file.ptr, .len = dirname_length };

		if(dirname_length > file.len) {
			return false;
		}

		tstr dup_dir = tstr_from_view(dirname);

		int dir_result = make_dir_recursive(tstr_data(&dup_dir));

		tstr_free(&dup_dir);

		if(dir_result != 0) {
			return false;
		}
	}

	int file_descriptor = open(file.ptr, O_WRONLY | O_CREAT, S_IRWXU);

	if(file_descriptor < 0) {
		return false;
	}

	const char* data = tstr_cstr(value);
	const size_t total_size = tstr_len(value);

	size_t written_bytes = 0;
	size_t remaining_bytes = total_size;

	while(true) {

		ssize_t write_result = write(file_descriptor, data + written_bytes, remaining_bytes);

		if(write_result < 0) {

			close(file_descriptor);
			return false;
		}

		if((size_t)write_result == remaining_bytes) {
			break;
		}

		if((size_t)write_result > remaining_bytes) {

			close(file_descriptor);
			return false;
		}

		written_bytes += (size_t)write_result;
		remaining_bytes -= (size_t)write_result;
	}

	int close_result = close(file_descriptor);

	return (close_result == 0);
}

NODISCARD ExitCode generate_variants(tstr_static input, tstr_static output) {

	if(input.ptr == NULL || output.ptr == NULL) {
		fprintf(stderr, "IMPLEMENTATION ERROR: input strings are <NULL>\n");
		return ExitCodeFailure;
	}

	if(!cwk_path_is_absolute(input.ptr)) {
		fprintf(stderr, "Input file path is not absolute: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		return ExitCodeFailure;
	}

	if(!cwk_path_is_absolute(output.ptr)) {
		fprintf(stderr, "Output file path is not absolute: " TSTR_FMT "\n",
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

	// TODO HERE

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
