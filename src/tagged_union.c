
#include "./tagged_union.h"

NODISCARD tstr_static get_tagged_unions(const tstr* input, TaggedUnionArray* array) {

	// TODO:
	UNUSED(input);
	*array = TVEC_EMPTY(TaggedUnion);

	return TSTR_STATIC_LIT("TODO");
}

void free_tagged_union_array(TaggedUnionArray* array) {
	// TODO
	UNUSED(array);
}
