

#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <tstr.h>

#include "./generate.h"
#include "./json.h"
#include "./utils.h"

#define IDENT1 "\t"
#define IDENT2 IDENT1 IDENT1

// prints the usage, if argc is not the right amount!
static void print_usage(FILE* stream, const tstr_static program_name) {

	fprintf(stream, "usage: " TSTR_FMT " <options>\n", TSTR_STATIC_FMT_ARGS(program_name));

	fprintf(stream, IDENT1 "options:\n");
	fprintf(stream, IDENT2 "-i, --input <input>: the input file in json format (required)\n");
	fprintf(stream, IDENT2 "-o, --output <output>: the output file (required)\n");
	fprintf(stream, IDENT1 "other:\n");
	fprintf(stream, IDENT2 "-h, -?, --help: print the help / usage\n");
	fprintf(stream, IDENT2 "-v, --version: print the version\n");
	fprintf(stream, IDENT2 "-g, -p, --get-schema, --print-schema, --variant-json-schema: print the "
	                       "schema for the input json file\n");
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

NODISCARD static bool is_schema_print(const tstr_static str) {
	if(tstr_static_eq(str, TSTR_STATIC_LIT("--get-schema"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("--print-schema"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("--variant-json-schema"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("-g"))) {
		return true;
	}

	if(tstr_static_eq(str, TSTR_STATIC_LIT("-p"))) {
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

NODISCARD static tstr generate_json_schema(void) {

	JsonSchema schema = get_json_schema_for_input();

	const tstr result = json_schema_to_string(&schema);

	free_json_schema(&schema);

	if(tstr_is_null(&result)) {
		return tstr_null();
	}

	return result;
}

static ExitCode rich_main(const ProgramArgs args) {
	if(args.size < 1) {
		fprintf(stderr, "No program name specified: FATAL ERROR\n");
		return ExitCodeFailure;
	}

	const tstr_static program_name = PROGRAM_ARGS_AT(args, 0);

	tstr_static input = tstr_static_null();

	tstr_static output = tstr_static_null();

	for(size_t i = 1; i < args.size; ++i) {
		const tstr_static arg = PROGRAM_ARGS_AT(args, i);

		if(is_help_string(arg)) {
			printf("General help menu:\n");
			print_usage(stdout, program_name);
			return ExitCodeSuccess;
		}

		if(is_version_string(arg)) {
			printf(STRINGIFY(_C_VARIANT_GENERATOR_VERSION_STRING) "\n");
			return ExitCodeSuccess;
		}

		if(is_schema_print(arg)) {
			tstr schema_as_string = generate_json_schema();
			if(tstr_is_null(&schema_as_string)) {
				fprintf(stderr, "failed to get schema as string\n");
				return ExitCodeFailure;
			}
			printf(TSTR_FMT "\n", TSTR_FMT_ARGS(schema_as_string));
			tstr_free(&schema_as_string);

			return ExitCodeSuccess;
		}

		if(tstr_static_eq(arg, TSTR_STATIC_LIT("-o")) ||
		   tstr_static_eq(arg, TSTR_STATIC_LIT("--output"))) {

			if(i + 1 >= args.size) {
				fprintf(stderr, "Expected another argument for the output argument\n");
				print_usage(stderr, program_name);
				return ExitCodeFailure;
			}

			const tstr_static value2 = PROGRAM_ARGS_AT(args, i + 1);

			if(!tstr_static_is_null(output)) {
				fprintf(stderr, "Output argument given twice\n");
				print_usage(stderr, program_name);
				return ExitCodeFailure;
			}

			output = value2;
			++i;
			continue;
		}

		if(tstr_static_eq(arg, TSTR_STATIC_LIT("-i")) ||
		   tstr_static_eq(arg, TSTR_STATIC_LIT("--input"))) {

			if(i + 1 >= args.size) {
				fprintf(stderr, "Expected another argument for the input argument\n");
				print_usage(stderr, program_name);
				return ExitCodeFailure;
			}

			const tstr_static value2 = PROGRAM_ARGS_AT(args, i + 1);

			if(!tstr_static_is_null(input)) {
				fprintf(stderr, "Input argument given twice\n");
				print_usage(stderr, program_name);
				return ExitCodeFailure;
			}

			input = value2;
			++i;
			continue;
		}

		fprintf(stderr, "Unrecognized argument: " TSTR_FMT "\n", TSTR_STATIC_FMT_ARGS(arg));
		print_usage(stderr, program_name);
		return ExitCodeFailure;
	}

	if(tstr_static_is_null(input)) {
		fprintf(stderr, "No input given\n");
		print_usage(stderr, program_name);
		return ExitCodeFailure;
	}

	if(tstr_static_is_null(output)) {
		fprintf(stderr, "No output given\n");
		print_usage(stderr, program_name);
		return ExitCodeFailure;
	}

	return generate_variants(input, output);
}

int main(const int argc, const char* const* const argv) {
	const ProgramArgs args = { .size = argc, .data = argv };
	return rich_main(args);
}
