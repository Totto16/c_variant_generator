#include "./colored_fprintf.h"

#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <tstr.h>

static int64_t fprintf_tstr_partial(FILE* stream, tstr_view str, va_list ap) {

	// TODO(Totto): fprintf needs a null terminated string, so we construct one here, but we should
	// make
	// this API alloc-less if possible, but that requires rewriting fprintf
	tstr str_alloc = tstr_from_view(str);

	if(tstr_is_null(&str_alloc)) {
		errno = ENOMEM;
		return -1;
	}
#if defined(__GNUC__) || defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
	int result = vfprintf(stream, tstr_cstr(&str_alloc), ap);

#if defined(__GNUC__) || defined(__clang__)
	#pragma clang diagnostic pop
#endif

	tstr_free(&str_alloc);

	return result;
}

static int64_t fprintf_single_color(FILE* stream, tstr_view str) {

	int result = fprintf(stream, "\33[" TSTR_FMT "m", TSV_FMT_ARGS(str));

	return result;
}

int64_t COLORED_FPRINTF_PRINTF_ATTR(2, 3) color_fprintf(FILE* stream, const char* const fmt, ...) {

	va_list args;

	bool tty = isatty(fileno(stream)) == 1;

	va_start(args, fmt);

	int64_t total_written = 0;
	size_t offs = 0;
	size_t i = 0;

	for(; fmt[i] != '\0'; ++i) {
		unsigned char character = fmt[i];

		if(character == COLORED_FPRINTF_COLOR_START_MARKER_CHAR) {
			// have a start, flush the previous string
			if(offs != i) {
				// we need to do something
				tstr_view view = { .data = fmt + offs, .len = i - offs };
				int64_t result = fprintf_tstr_partial(stream, view, args);
				if(result < 0) {
					return result;
				}
				total_written += result;
			}
			const size_t start_idx = i;

			// now search for the end character

			size_t end_idx = start_idx;

			for(++i; fmt[i] != '\0'; ++i) {
				unsigned char character_2 = fmt[i];
				if(character_2 == COLORED_FPRINTF_COLOR_START_MARKER_CHAR) {
					// error, expected end first
					errno = EINVAL;
					return -1;
				} else if(character_2 == COLORED_FPRINTF_COLOR_END_MARKER_CHAR) {
					end_idx = i;
					break;
				}
			}

			if(end_idx == start_idx) {
				// we didn't find the end
				errno = EINVAL;
				return -1;
			}

			if(tty) {
				tstr_view color_info = { .data = fmt + start_idx + 1,
					                     .len = end_idx - start_idx - 2 };

				// print the color
				int64_t result = fprintf_single_color(stream, color_info);
				if(result < 0) {
					return result;
				}
				total_written += result;
			}

			offs = i;
		}
	}

	// if we have something left, print it
	if(offs != i) {
		// we need to do something
		tstr_view view = { .data = fmt + offs, .len = i - offs };
		int64_t result = fprintf_tstr_partial(stream, view, args);
		if(result < 0) {
			return result;
		}
		total_written += result;
	}

	va_end(args);
	return total_written;
}
