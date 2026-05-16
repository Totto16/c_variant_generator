#pragma once

#include <tstr.h>
#include <tvec.h>

#include "./utils.h"

typedef struct {
	int todo;
} TaggedUnion;

TVEC_DEFINE_VEC_TYPE(TaggedUnion)

typedef TVEC_TYPENAME(TaggedUnion) TaggedUnionArray;

NODISCARD tstr_static get_tagged_unions(const tstr* input, TaggedUnionArray* array);

void free_tagged_union_array(TaggedUnionArray* array);
