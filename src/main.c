

#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <tstr.h>

#include "./generate.h"
#include "./utils.h"

#define IDENT1 "\t"
#define IDENT2 IDENT1 IDENT1

// prints the usage, if argc is not the right amount!
static void print_usage(const tstr_static program_name) {

	printf("usage: " TSTR_FMT " <input> <output>\n", TSTR_STATIC_FMT_ARGS(program_name));

	printf(IDENT1 "input: the input file in json format (required)\n");
	printf(IDENT1 "output: the output file (required)\n");
}

NODISCARD static bool is_help_string(const tstr_static str) {
	if(tstr_static_eq(str, TSTR_STATIC_LIT("--help"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("-h"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("-?"))) {
		return true;
	}

	return false;
}

NODISCARD static bool is_version_string(const tstr_static str) {
	if(tstr_static_eq(str, TSTR_STATIC_LIT("--version"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("-v"))) {
		return true;
	}

	return false;
}

typedef struct {
	size_t size;
	const char* const* data;
} ProgramArgs;

#ifdef NDEBUG
static void assert_fn(const char* const expr, bool value) {
	OOM_ASSERT(value, expr);
}
	#define ASSERT_EXPR(expr) assert_fn(#expr, expr)
#else
	#define ASSERT_EXPR(expr) assert(expr)
#endif

#define PROGRAM_ARGS_AT(args, index) \
	(ASSERT_EXPR((index) < (args).size), tstr_static_from_static_cstr((args).data[(index)]))

void make_expr(void);

static ExitCode rich_main(const ProgramArgs args) {
	if(args.size < 1) {
		fprintf(stderr, "No program name specified: FATAL ERROR\n");
		return ExitCodeFailure;
	}

	const tstr_static program_name = PROGRAM_ARGS_AT(args, 0);

	// checking if there are enough arguments
	if(args.size < 3) {

		if(args.size > 1) {

			const tstr_static first_val = PROGRAM_ARGS_AT(args, 1);

			if(is_help_string(first_val)) {
				printf("General help menu:\n");
				print_usage(program_name);
				return ExitCodeSuccess;
			}

			if(is_version_string(first_val)) {
				printf(STRINGIFY(_C_VARIANT_GENERATOR_VERSION_STRING) "\n");
				return ExitCodeSuccess;
			}
		}

		fprintf(stderr, "Not enough arguments specified\n");
		print_usage(program_name);
		return ExitCodeFailure;
	}

	const tstr_static input = PROGRAM_ARGS_AT(args, 1);

	const tstr_static output = PROGRAM_ARGS_AT(args, 2);

	return generate_variants(input, output);
}

int main(const int argc, const char* const* const argv) {
	const ProgramArgs args = { .size = argc, .data = argv };
	return rich_main(args);
}
