#pragma once

#include <tstr.h>

#include "./utils.h"

typedef struct CaseNameImpl CaseName;

NODISCARD CaseName* case_name_from_parts(const tstr_static**** ARRAY_TODO);

NODISCARD CaseName* case_name_from_PascalCase(const tstr* snake_case);

NODISCARD CaseName* case_name_from_snake_case(const tstr* snake_case);

NODISCARD CaseName* case_name_combine(const CaseName* one, const CaseName* two);

NODISCARD bool case_name_eq(const CaseName* one, const CaseName* two);

NODISCARD const tstr* case_name_get_PascalCase(const CaseName* name);

NODISCARD const tstr* case_name_get_MACOR_NAME(const CaseName* name);

NODISCARD const tstr* case_name_get_snake_case(const CaseName* name);

void case_name_free(CaseName* name);
