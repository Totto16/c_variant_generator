
#include "./generate.h"
#include "./case_name.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cwalk.h>
#include <tstr_builder.h>

NODISCARD static tstr_view cwk_wrapper_get_ext(const tstr* value) {

	const char* ext = NULL;
	size_t len = 0;
	bool result = cwk_path_get_extension(tstr_cstr(value), &ext, &len);

	if(!result) {
		return TSTR_EMPTY_VIEW;
	}

	return (tstr_view){ .data = ext, .len = len };
}

NODISCARD static tstr cwk_wrapper_make_absolute(tstr_static path) {

	if(cwk_path_is_absolute(path.ptr)) {
		return tstr_from_static_tstr(path);
	}

	char cwd_buffer[FILENAME_MAX];

	char* cwd = getcwd(cwd_buffer, sizeof(cwd_buffer) / sizeof(*cwd_buffer));

	if(cwd == NULL) {
		return tstr_null();
	}

	char buffer[FILENAME_MAX];

	const size_t buffer_size = sizeof(buffer) / sizeof(*buffer);

	const size_t path_size = cwk_path_get_absolute(cwd, path.ptr, buffer, sizeof(buffer));

	if(path_size >= buffer_size) {
		return tstr_null();
	}

	return tstr_from_view((tstr_view){ .data = buffer, .len = path_size });
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

		if(dirname.len > file.len) {
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

NODISCARD static CaseName* get_file_name_as_case_name(const tstr* file) {

	const char* basename_ptr = NULL;
	size_t length = 0;

	cwk_path_get_basename(tstr_cstr(file), &basename_ptr, &length);

	if(basename_ptr == NULL || length == 0) {
		return NULL;
	}

	const tstr_static basename = { .ptr = basename_ptr, .len = length };

	const tstr basename_tstr = tstr_from_static_tstr(basename);

	tstr_view ext = cwk_wrapper_get_ext(&basename_tstr);

	const tstr_static name = { .ptr = basename_ptr, .len = length - ext.len };

	const tstr name_tstr = tstr_from_static_tstr(name);

	return case_name_from_snake_case(&name_tstr);
}

NODISCARD ExitCode generate_variants(tstr_static input, tstr_static output) {

	if(input.ptr == NULL || output.ptr == NULL) {
		fprintf(stderr, "IMPLEMENTATION ERROR: input strings are <NULL>\n");
		return ExitCodeFailure;
	}

#define FREE_AT_END() \
	do { \
	} while(false)

	tstr input_abs = cwk_wrapper_make_absolute(input);

	if(tstr_is_null(&input_abs)) {
		fprintf(stderr, "Input file path can't be made absolute: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		FREE_AT_END();
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&input_abs); \
	} while(false)

	tstr output_abs = cwk_wrapper_make_absolute(output);

	if(tstr_is_null(&output_abs)) {
		fprintf(stderr, "Output file path can't be made absolute: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(output));
		FREE_AT_END();
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&output_abs); \
		tstr_free(&input_abs); \
	} while(false)

	const tstr_view input_extension = cwk_wrapper_get_ext(&input_abs);

	if(!tstr_view_eq_view(input_extension, TSTR_TSV(".json"))) {
		fprintf(stderr, "Input file has not a .json extension:" TSTR_FMT "\n",
		        TSV_FMT_ARGS(input_extension));
		FREE_AT_END();
		return ExitCodeFailure;
	}

	const tstr_view output_extension = cwk_wrapper_get_ext(&output_abs);

	if(!tstr_view_eq_view(output_extension, TSTR_TSV(".h"))) {
		fprintf(stderr, "Output file has not a .h extension:" TSTR_FMT "\n",
		        TSV_FMT_ARGS(output_extension));
		FREE_AT_END();
		return ExitCodeFailure;
	}

	StringBuilder* string_builder = string_builder_init();

	if(string_builder == NULL) {
		FREE_AT_END();
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		free_string_builder(string_builder); \
		tstr_free(&output_abs); \
		tstr_free(&input_abs); \
	} while(false)

	CaseName* file_name = get_file_name_as_case_name(&output_abs);

	if(file_name == NULL) {
		fprintf(stderr, "Error: can't get the CaseName for the filename\n");
		FREE_AT_END();
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		case_name_free(file_name); \
		free_string_builder(string_builder); \
		tstr_free(&output_abs); \
		tstr_free(&input_abs); \
	} while(false)

	// TODO HERE

	tstr value = string_builder_release_into_tstr(&string_builder);

	if(tstr_is_null(&value)) {
		fprintf(stderr, "Error: couldn't get string from string builder\n");
		FREE_AT_END();
		return ExitCodeFailure;
	}

#undef FREE_AT_END
#define FREE_AT_END() \
	do { \
		tstr_free(&value); \
		case_name_free(file_name); \
		free_string_builder(string_builder); \
		tstr_free(&output_abs); \
		tstr_free(&input_abs); \
	} while(false)

	bool write_result = write_file_helper(output, &value);

	if(!write_result) {
		fprintf(stderr, "Error: couldn't write to file: " TSTR_FMT "\n",
		        TSTR_STATIC_FMT_ARGS(input));
		FREE_AT_END();
		return ExitCodeFailure;
	}

	FREE_AT_END();

	return ExitCodeSuccess;
}

#undef FREE_AT_END
