#include "./case_name.h"

TVEC_IMPLEMENT_VEC_TYPE(tstr)


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
	return get_case_name_impl(PascalCase, MACRO_NAME, snake_case);
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

NODISCARD static bool is_utf8_string(const tstr* str) {
	// TODO:
	(void)str;
	return false;
}

NODISCARD CaseName* case_name_from_PascalCase(const tstr* snake_case) {
	if(!is_utf8_string(snake_case)) {
		// TODO:`Unicode strings not yet supported: ${str}`)
		return NULL;
	}

	TstrArray array = TVEC_EMPTY(tstr);

	tstr_view current = { .data = tstr_cstr(snake_case), .len = 0 };

	for(size_t i = 0; i < tstr_len(snake_case); ++i) {
		char ch = tstr_cstr(snake_case)[i];

		if(current.len == 0) {
			if(!isupper(ch)) {
				free_tstr_array(&array);
				return NULL;
			}

			current.len += 1;

		} else {

			if(isupper(ch)) {
				if(current.len <= 1) {
					free_tstr_array(&array);
					return NULL;
				}

				tstr allocated = tstr_from_view(current);

				assert(tstr_len(&allocated) > 1);

				tstr_data(&allocated)[0] = tolower(tstr_data(&allocated)[0]);

				TvecResult push_res = TVEC_PUSH(tstr, &array, allocated);

				if(push_res != TvecResultOk) {
					tstr_free(&allocated);
					free_tstr_array(&array);
					return NULL;
				}

				current.data = current.data + current.len;
				current.len = 0;

			} else {
				current.len += 1;
			}
		}
	}

	return case_name_from_sanitized_parts(&array);
}

NODISCARD CaseName* case_name_from_snake_case(const tstr* snake_case) {

	if(!is_utf8_string(snake_case)) {
		// TODO:`Unicode strings not yet supported: ${str}`)
		return NULL;
	}

	TstrArray array = TVEC_EMPTY(tstr);

	tstr_split_iter iter = tstr_split_init(tstr_as_view(snake_case), "_");

	while(!iter.finished) {

		tstr_view out = TSTR_EMPTY_VIEW;

		bool successfull = tstr_split_next(&iter, &out);

		if(!successfull) {
			free_tstr_array(&array);
			return NULL;
		}

		for(size_t i = 0; i < out.len; ++i) {
			char ch = out.data[i];

			if(!islower(ch)) {
				free_tstr_array(&array);
				return NULL;
			}
		}

		tstr allocated = tstr_from_view(out);

		TvecResult push_res = TVEC_PUSH(tstr, &array, allocated);

		if(push_res != TvecResultOk) {
			tstr_free(&allocated);
			free_tstr_array(&array);
			return NULL;
		}
	}

	return case_name_from_sanitized_parts(&array);
}

NODISCARD CaseName* case_name_combine(const CaseName* one, const CaseName* two) {

	tstr PascalCase = tstr_init();
	tstr MACRO_NAME = tstr_init();
	tstr snake_case = tstr_init();

#define FREE_AT_END() \
	do { \
		tstr_free(&PascalCase); \
		tstr_free(&MACRO_NAME); \
		tstr_free(&snake_case); \
	} while(false)

	{ // PascalCase
		TStrResult cat_result =
		    tstr_cat_len(&PascalCase, tstr_cstr(&(one->PascalCase)), tstr_len(&(one->PascalCase)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}

		cat_result =
		    tstr_cat_len(&PascalCase, tstr_cstr(&(two->PascalCase)), tstr_len(&(two->PascalCase)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}
	}

	{ // MACRO_NAME
		TStrResult cat_result =
		    tstr_cat_len(&MACRO_NAME, tstr_cstr(&(one->MACRO_NAME)), tstr_len(&(one->MACRO_NAME)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}

		if(tstr_len(&MACRO_NAME) > 0) {
			cat_result = tstr_push_char(&MACRO_NAME, '_');

			if(cat_result != TStrResultOk) {
				FREE_AT_END();
				return NULL;
			}
		}

		cat_result =
		    tstr_cat_len(&MACRO_NAME, tstr_cstr(&(two->MACRO_NAME)), tstr_len(&(two->MACRO_NAME)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}
	}

	{ // snake_case
		TStrResult cat_result =
		    tstr_cat_len(&snake_case, tstr_cstr(&(one->snake_case)), tstr_len(&(one->snake_case)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}

		if(tstr_len(&snake_case) > 0) {
			cat_result = tstr_push_char(&snake_case, '_');

			if(cat_result != TStrResultOk) {
				FREE_AT_END();
				return NULL;
			}
		}

		cat_result =
		    tstr_cat_len(&snake_case, tstr_cstr(&(two->snake_case)), tstr_len(&(two->snake_case)));

		if(cat_result != TStrResultOk) {
			FREE_AT_END();
			return NULL;
		}
	}

	return get_case_name_impl(PascalCase, MACRO_NAME, snake_case);
}

#undef FREE_AT_END

NODISCARD bool case_name_eq(const CaseName* one, const CaseName* two) {
	if(!tstr_eq(&(one->PascalCase), &(two->PascalCase))) {
		return false;
	}

	if(!tstr_eq(&(one->MACRO_NAME), &(two->MACRO_NAME))) {
		return false;
	}

	return tstr_eq(&(one->snake_case), &(two->snake_case));
}

NODISCARD const tstr* case_name_get_PascalCase(const CaseName* name) {
	if(name == NULL) {
		return NULL;
	}

	const tstr* result = &(name->PascalCase);

	if(tstr_is_null(result)) {
		return NULL;
	}

	return result;
}

NODISCARD const tstr* case_name_get_MACRO_NAME(const CaseName* name) {
	if(name == NULL) {
		return NULL;
	}

	const tstr* result = &(name->MACRO_NAME);

	if(tstr_is_null(result)) {
		return NULL;
	}

	return result;
}

NODISCARD const tstr* case_name_get_snake_case(const CaseName* name) {
	if(name == NULL) {
		return NULL;
	}

	const tstr* result = &(name->snake_case);

	if(tstr_is_null(result)) {
		return NULL;
	}

	return result;
}

void case_name_free(CaseName* name) {
	if(name == NULL) {
		return;
	}

	tstr_free(&(name->PascalCase));
	tstr_free(&(name->MACRO_NAME));
	tstr_free(&(name->snake_case));
}
