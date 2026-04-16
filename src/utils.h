
#pragma once

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// see
// https://clang.llvm.org/docs/AttributeReference.html#nullability-attributes

#if defined(__GNUC__) || defined(__clang__)
	#if defined(__clang__)
		#define NULLABLE _Nullable
		#define NON_NULLABLE _Nonnull
		#define NO_NULLABLE_INFO _Null_unspecified
	#else
		#define NULLABLE
		#define NON_NULLABLE
		#define NO_NULLABLE_INFO
	#endif

	#define OUT_PARAM(type) type* NON_NULLABLE
	#define DEFAULT_PARAM(type) type* NO_NULLABLE_INFO
#elif defined(_MSC_VER)
	#define NULLABLE
	#define NON_NULLABLE
	#define NO_NULLABLE_INFO

	#define OUT_PARAM(type) _Out_ type*
	#define DEFAULT_PARAM(type) type*

#else
	#define NULLABLE
	#define NON_NULLABLE
	#define NO_NULLABLE_INFO

	#define OUT_PARAM(type) type*
	#define DEFAULT_PARAM(type) type*
#endif

#if _C_VARIANT_GENERATOR_COMPILE_WITH_NARROWED_ENUMS
	#define C_23_NARROW_ENUM_TO(x) : x
	#define C_23_ENUM_TYPE(x) x

	#define VARIANT_IMPL_ALL_USE_NARROWED_ENUMS 1
#else
	#define C_23_NARROW_ENUM_TO(x)
	#define C_23_ENUM_TYPE(x) int

	#define VARIANT_IMPL_ALL_USE_NARROWED_ENUMS 0
#endif

#define VARIANT_IMPL_ALL_VARIANTS_COMPILED_WITH_NARROWED_ENUMS VARIANT_IMPL_ALL_USE_NARROWED_ENUMS
#define VARIANT_IMPL_GENERATED_VARIANTS_COMPILED_WITH_NARROWED_ENUMS \
	VARIANT_IMPL_ALL_USE_NARROWED_ENUMS

#if defined(__clang__)
    // see: https://clang.llvm.org/docs/AttributeReference.html#flag-enum
	#define ENUM_IS_MASK __attribute__((flag_enum))
#else
	#define ENUM_IS_MASK
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202000) || __cplusplus
	#define NODISCARD [[nodiscard]]
	#define MAYBE_UNUSED [[maybe_unused]]
#else
    // see e.g. https://www.gnu.org/software/gnulib/manual/html_node/Attributes.html
	#define NODISCARD __attribute__((__warn_unused_result__))
	#define MAYBE_UNUSED __attribute__((__unused__))
#endif

#define UNUSED(v) ((void)(v))

/**
 * @enum value
 */
typedef enum C_23_NARROW_ENUM_TO(uint8_t) {
	ExitCodeFailure = EXIT_FAILURE,
	ExitCodeSuccess = EXIT_SUCCESS,
} ExitCode;

// cool trick from here:
// https://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build
#ifdef NDEBUG
	#define assert(x) /* NOLINT(readability-identifier-naming) */ \
		do { \
			UNUSED((x)); \
		} while(false)

	#define UNREACHABLE() \
		do { \
			fprintf(stderr, "[%s %s:%d]: UNREACHABLE\n", __func__, __FILE__, __LINE__); \
			exit(ExitCodeFailure); \
		} while(false)

	#define OOM_ASSERT(value, message) \
		do { \
			if(!(value)) { \
				fprintf(stderr, "[%s %s:%d]: OOM: %s\n", __func__, __FILE__, __LINE__, (message)); \
				exit(ExitCodeFailure); \
			} \
		} while(false)

#else
	#include <assert.h>

	#define UNREACHABLE() \
		do { \
			assert(false && "UNREACHABLE"); /* NOLINT(cert-dcl03-c,misc-static-assert) */ \
		} while(false)

	#define OOM_ASSERT(value, message) \
		do { \
			assert((value) && (message)); /* NOLINT(cert-dcl03-c,misc-static-assert) */ \
		} while(false)

#endif

// uses snprintf feature with passing NULL,0 as first two arguments to
// automatically determine the required buffer size, for more read man page for
// variadic functions its easier to use macro magic, attention, use this
// function in the right way, you have to prepare a char* that is set to null,
// then it works best! snprintf is safer then sprintf, since it guarantees some
// things, also it has a failure indicator
#define FORMAT_STRING_IMPL(toStore, statement, logger_fn, format, ...) \
	{ \
		char* internalBuffer = *toStore; \
		if(internalBuffer != NULL) { \
			free(internalBuffer); \
		} \
		const LibCInt toWrite = snprintf(NULL, 0, format, __VA_ARGS__) + 1; \
		internalBuffer = (char*)malloc(toWrite * sizeof(char)); \
		if(!internalBuffer) { \
			logger_fn("Couldn't allocate memory for %d bytes!\n", toWrite); \
			statement \
		} \
		const LibCInt written = snprintf(internalBuffer, toWrite, format, __VA_ARGS__); \
		if(written >= toWrite) { \
			logger_fn("snprintf did write more bytes then it had space in the " \
			          "buffer, available " \
			          "space: '%d', actually written: '%d'!\n", \
			          (toWrite) - 1, written); \
			free(internalBuffer); \
			statement \
		} \
		*toStore = internalBuffer; \
	} \
	if(*toStore == NULL) { \
		logger_fn("snprintf Macro gone wrong: '%s' is pointing to NULL!\n", #toStore); \
		statement \
	}

#define FORMAT_STRING(toStore, statement, format, ...) \
	FORMAT_STRING_IMPL(toStore, statement, IMPL_STDERR_LOGGER, format, __VA_ARGS__)

#define IMPL_STDERR_LOGGER(format, ...) fprintf(stderr, format, __VA_ARGS__)

#define TSTR_KEYNAME TString

#define STRINGIFY(a) STR_IMPL(a)
#define STR_IMPL(a) #a

#define ZERO_STRUCT(Type) (Type){ 0 }
#define ZERO_ARRAY() { 0 }
