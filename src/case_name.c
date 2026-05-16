#include "./case_name.h"

struct CaseNameImpl {
	// NOTE. JS uses a array of the parts, normalized, but than it needed to allocate on every call
	// to getter functions, so we store all needed variants inline and the getter functions just
	// return them
	tstr PascalCase;
	tstr MACRO_NAME;
	tstr snake_case;
};

NODISCARD static CaseName* get_case_name_impl(tstr PascalCase, tstr MACRO_NAME, tstr snake_case) {
	CaseName* case_name = malloc(sizeof(CaseName));

	if(case_name == NULL) {
		return NULL;
	}

	*case_name = (CaseName){
		.PascalCase = PascalCase,
		.MACRO_NAME = MACRO_NAME,
		.snake_case = snake_case,
	};

	return case_name;
}

static void free_tstr_array(TstrArray* const array) {
	for(size_t i = 0; i < TVEC_LENGTH(tstr, *array); ++i) {
		tstr* value = TVEC_GET_AT_MUT(tstr, array, i);
		tstr_free(value);
	}

	TVEC_FREE(tstr, array);
	*array = TVEC_EMPTY(tstr);
}

NODISCARD static CaseName* case_name_from_sanitized_parts(TstrArray* array) {

	tstr PascalCase = tstr_init();
	tstr MACRO_NAME = tstr_init();
	tstr snake_case = tstr_init();

	size_t total_size = 0;

	const size_t array_size = TVEC_LENGTH(tstr, *array);

	for(size_t i = 0; i < array_size; ++i) {
		const tstr value = TVEC_AT(tstr, *array, i);

		total_size += tstr_len(&value);
	}

	TStrResult reserve_result = tstr_reserve(&PascalCase, total_size);

	if(reserve_result != TStrResultOk) {
		free_tstr_array(array);
		return NULL;
	}

	const size_t size_2 = total_size + (array_size == 0 ? 0 : array_size - 1);

	reserve_result = tstr_reserve(&MACRO_NAME, size_2);

	if(reserve_result != TStrResultOk) {
		free_tstr_array(array);
		return NULL;
	}

	reserve_result = tstr_reserve(&snake_case, size_2);

	if(reserve_result != TStrResultOk) {
		free_tstr_array(array);
		return NULL;
	}

	size_t total_idx = 0;
	size_t total_idx2 = 0;

	for(size_t i = 0; i < array_size; ++i) {
		const tstr value = TVEC_AT(tstr, *array, i);

		for(size_t j = 0; j < tstr_len(&value); ++j) {
			char ch = tstr_cstr(&value)[j];

			char lch = tolower(ch);
			char uch = toupper(ch);

			tstr_data(&PascalCase)[total_idx] = j == 0 ? uch : lch;

			tstr_data(&MACRO_NAME)[total_idx2] = uch;
			tstr_data(&snake_case)[total_idx2] = lch;

			++total_idx;
			++total_idx2;
		}

		if(i + 1 < array_size) {
			tstr_data(&MACRO_NAME)[total_idx2] = '_';
			tstr_data(&snake_case)[total_idx2] = '_';
			++total_idx2;
		}
	}

	free_tstr_array(array);
	return NULL;
}

NODISCARD CaseName* case_name_from_parts(TstrArray* array) {

	for(size_t i = 0; i < TVEC_LENGTH(tstr, *array); ++i) {
		const tstr value = TVEC_AT(tstr, *array, i);

		for(size_t j = 0; j < tstr_len(&value); ++j) {
			char ch = tstr_cstr(&value)[j];

			if(!isascii(ch)) {
				free_tstr_array(array);
				return NULL;
			}
		}
	}

	return case_name_from_sanitized_parts(array);
}

NODISCARD CaseName* case_name_from_PascalCase(const tstr* snake_case) {
	// TODO
}

NODISCARD CaseName* case_name_from_snake_case(const tstr* snake_case) {
	// TODO
}

NODISCARD CaseName* case_name_combine(const CaseName* one, const CaseName* two) {
	// TODO
}

NODISCARD bool case_name_eq(const CaseName* one, const CaseName* two) {
	// TODO
}

NODISCARD const tstr* case_name_get_PascalCase(const CaseName* name) {
	// TODO
}

NODISCARD const tstr* case_name_get_MACOR_NAME(const CaseName* name) {
	// TODO
}

NODISCARD const tstr* case_name_get_snake_case(const CaseName* name) {
	// TODO
}

void case_name_free(CaseName* name) {
	// TODO
}
