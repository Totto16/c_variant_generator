#pragma once

#include "./utils.h"
#include <tstr.h>
#include <tvec.h>

typedef struct CaseNameImpl CaseName;

TVEC_DEFINE_VEC_TYPE(tstr)

typedef TVEC_TYPENAME(tstr) TstrArray;

NODISCARD CaseName* case_name_from_parts(TstrArray* array, tstr_static* err);

NODISCARD CaseName* case_name_from_PascalCase(const tstr* snake_case, tstr_static* err);

NODISCARD CaseName* case_name_from_snake_case(const tstr* snake_case, tstr_static* err);

NODISCARD CaseName* case_name_combine(const CaseName* one, const CaseName* two);

NODISCARD bool case_name_eq(const CaseName* one, const CaseName* two);

NODISCARD const tstr* case_name_get_PascalCase(const CaseName* name);

NODISCARD const tstr* case_name_get_MACRO_NAME(const CaseName* name);

NODISCARD const tstr* case_name_get_snake_case(const CaseName* name);

void case_name_free(CaseName* name);
