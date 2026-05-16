#include "./case_name.h"

struct CaseNameImpl {
	// NOTE. JS uses a array of the parts, normalized, but than it needed to allocate on every call
	// to getter functions, so we store all needed variants inline and the getter functions just
	// return them
	tstr PascalCase;
	tstr MACRO_NAME;
	tstr snake_case;
};

NODISCARD CaseName* case_name_from_parts(const tstr_static**** ARRAY_TODO) {

	// TODO
	UNUSED(ARRAY_TODO);
	return NULL;
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
