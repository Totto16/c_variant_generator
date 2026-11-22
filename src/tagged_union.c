
#include "./tagged_union.h"
#include "./colored_fprintf.h"
#include "./json.h"

#include <tjson.h>
#include <tjson_helper.h>

#include <fcntl.h>
#include <unistd.h>

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnionArray)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnion)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedMemberArray)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedMember)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedType)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(CAnonymousStruct)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(StructMemberArray)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(StructMember)

TRTTI_DEFINE_TYPE_AS_SUPPORTED(tstr)

TVEC_IMPLEMENT_VEC_TYPE(TaggedUnion)

TVEC_IMPLEMENT_VEC_TYPE(TaggedMember)

TVEC_IMPLEMENT_VEC_TYPE(StructMember)

typedef struct {
	CaseName* name;
} CaseNameWrapper;

TRTTI_DEFINE_TYPE_AS_SUPPORTED(CaseNameWrapper)

#define TAGGED_NAME_NULL(Name, name_member) \
	((Name){ .name = NULL, .TAGE_NAME_IMPL(name_member) = {} })

#define TAGGED_UNION_ENUM_NULL() \
	((TaggedUnionEnum){ \
	    .name = TAGGED_NAME_NULL(EnumName, enum), \
	    .underlying_type = CEnumTypeBestMatch, \
	})

#define CPP_FEATURES_NULL() \
	((CppFeatures){ \
	    .tag_as_error_variant = false, \
	})

#define TAGGED_UNION_REQUIREMENTS_NULL() \
	((TaggedUnionRequirements){ \
	    .order = StructOrderRequirementBestSize, \
	})

#define TAGGED_UNION_OPTIONS_NULL() \
	((TaggedUnionOptions){ \
	    .raw_struct = TAGGED_NAME_NULL(RawStructName, raw_struct), \
	    .struct_order = StructOrderAuto, \
	    .requirements = TAGGED_UNION_REQUIREMENTS_NULL(), \
	    .cpp_features = CPP_FEATURES_NULL(), \
	})

#define TAGGED_UNION_NULL() \
	((TaggedUnion){ \
	    .name = TAGGED_NAME_NULL(UnionName, union), \
	    .member = TVEC_EMPTY(TaggedMember), \
	    .enum_props = TAGGED_UNION_ENUM_NULL(), \
	    .options = TAGGED_UNION_OPTIONS_NULL(), \
	})

#define TAGGED_TYPE_NULL() ((TaggedType){ .type = TaggedTypeTypeNull, .data = {} })

#define TAGGED_MEMBER_NULL() \
	((TaggedMember){ \
	    .name = TAGGED_NAME_NULL(MemberName, member), \
	    .type = TAGGED_TYPE_NULL(), \
	})

#define C_ANONYMOUS_STRUCT_NULL() ((CAnonymousStruct){ .members = TVEC_EMPTY(StructMember) })

#define STRUCT_MEMBER_NULL() \
	((StructMember){ \
	    .name = tstr_null(), \
	    .type_name = tstr_null(), \
	})

typedef struct {
	size_t struct_id;
} TaggedUnionProcessUserdata;

TRTTI_DEFINE_TYPE_AS_SUPPORTED(TaggedUnionProcessUserdata)

NODISCARD tstr_static tagged_type_type_as_string(TaggedTypeType type) {
	switch(type) {
		case TaggedTypeTypeNull: {
			return TSTR_STATIC_LIT("null");
		}
		case TaggedTypeTypeSimple: {
			return TSTR_STATIC_LIT("simple");
		}
		case TaggedTypeTypeStruct: {
			return TSTR_STATIC_LIT("struct");
		}
		default: {
			return TSTR_STATIC_LIT("<unknown>");
		}
	}
}

static JsonIterateResult json_value_iterator_process_tagged_unions(const JsonPath* path,
                                                                   RTTIAnnotatedValue parent,
                                                                   JsonIterateValue value,
                                                                   RTTIAnnotatedValue userdata) {

	TaggedUnionProcessUserdata* union_userdata =
	    TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionProcessUserdata, userdata);

	UNUSED(union_userdata);

	if(json_path_is_root(path)) {

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {
			// we already "allocated" (initialized the array), nothing to do here

			if(!TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, parent)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err =
				        TSTR_LIT("implementation error, first RTTI value not 'TaggedUnionArray'"),
				    .path = NULL,
				});
			}

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(

		    "Error: unhandled iterate value root path: <no more info>",
		    "Error: unhandled iterate value in root path: " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, parent)) {

		TaggedUnionArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionArray, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			TaggedUnion* union_slot = TVEC_PUSH_SLOT(TaggedUnion, array);

			if(union_slot == NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*union_slot = TAGGED_UNION_NULL();

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TaggedUnion, union_slot);

			return new_json_iterate_result_ok(result);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'TaggedUnionArray': <no more info>",
		    "Error: unhandled iterate value at 'TaggedUnionArray': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnion, parent)) {

		TaggedUnion* tagged_union = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnion, parent);

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {

			// TODO: assert that the key is "member"
			// TODO: assert that its not a duplicate, e.g. that member is empty (empty array)

			tagged_union->member = TVEC_EMPTY(TaggedMember);

			TaggedMemberArray* member_slot = &(tagged_union->member);

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TaggedMemberArray, member_slot);

			return new_json_iterate_result_ok(result);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'TaggedUnion': <no more info>",
		    "Error: unhandled iterate value at 'TaggedUnion': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedMemberArray, parent)) {

		TaggedMemberArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedMemberArray, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			TaggedMember* member_slot = TVEC_PUSH_SLOT(TaggedMember, array);

			if(member_slot == NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*member_slot = TAGGED_MEMBER_NULL();

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TaggedMember, member_slot);

			return new_json_iterate_result_ok(result);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'TaggedMemberArray': <no more info>",
		    "Error: unhandled iterate value at 'TaggedMemberArray': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedMember, parent)) {

		TaggedMember* member = TRTTI_ANNOTATED_VALUE_CAST(TaggedMember, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			// TODO: assert that the key is "type"
			// TODO: assert that its not a duplicate, e.g. that type is empty

			member->type = TAGGED_TYPE_NULL();

			TaggedType* type_slot = &(member->type);

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(TaggedType, type_slot);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("type"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(TaggedType, object_entry.entry.value)) {

					// note if we use allocated data and add here here, we could add the data here,
					// but we
					// use a struct slot algorithm, so nothing to do here, we could validate the
					// finished entry if we like

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC(
				    "Error: invalid json value for type that expected: 'TaggedType': <no "
				    "more info>",
				    "Error: invalid json value for type that expected: "
				    "'TaggedType': " TRTTI_FMT,
				    TRTTI_FMT_ARGS(object_entry.entry.value));

			} else if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("name"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(CaseNameWrapper, object_entry.entry.value)) {

					CaseNameWrapper* case_name_wrapper =
					    TRTTI_ANNOTATED_VALUE_CAST(CaseNameWrapper, object_entry.entry.value);
					if(case_name_wrapper == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content"),
						    .path = NULL,
						});
					}

					if(member->name.name != NULL) {

						free_case_name(case_name_wrapper->name);
						TJSON_FREE(case_name_wrapper);
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT("implementation error, set type_name twice"),
						    .path = NULL,
						});
					}

					member->name = GET_TAGGED_NAME(MemberName, member, case_name_wrapper->name);
					;
					TJSON_FREE(case_name_wrapper);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC(
				    "Error: invalid json value for type that expected: 'CaseNameWrapper': <no "
				    "more info>",
				    "Error: invalid json value for type that expected: "
				    "'CaseNameWrapper': " TRTTI_FMT,
				    TRTTI_FMT_ARGS(object_entry.entry.value));

			}

			else if(tstr_is_null(object_entry.entry.key)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, the object entry key is <NULL>"),
				    .path = NULL,
				});
			}

			RETURN_JSON_ITERATE_ERROR_DYNAMIC(
			    "Error: unhandled object key at 'TaggedMember': <no more info>",
			    "Error: unhandled object key at 'TaggedMember': " TSTR_FMT,
			    TSTR_FMT_ARGS(*object_entry.entry.key));
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return NON empty, as this is the result of the subparser, so the return
			// value of this is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		IF_JSON_ITERATE_VALUE_IS_STRING_CONST(value) {

			tstr str_value = json_string_get_as_str(string);

			if(tstr_is_null(&str_value)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("error in getting tstr from json string"),
				    .path = NULL,
				});
			}
			tstr_static case_name_err = TSTR_STATIC_LIT("");

			CaseName* case_name = case_name_from_PascalCase(&str_value, &case_name_err);

			if(case_name == NULL) {
				RETURN_JSON_ITERATE_ERROR_DYNAMIC_EXTENDED(
				    NULL, tstr_free(&str_value), "Error: invalid CaseName: <no more info>",
				    "Error: invalid CaseName: " TSTR_FMT " -> value: " TSTR_FMT,
				    TSTR_STATIC_FMT_ARGS(case_name_err), TSTR_FMT_ARGS(str_value));
			}

			tstr_free(&str_value);

			CaseNameWrapper* case_name_wrapper = TJSON_NEW(CaseNameWrapper);

			if(case_name_wrapper == NULL) {
				free_case_name(case_name);
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*case_name_wrapper = (CaseNameWrapper){ .name = case_name };

			RTTIAnnotatedValue result =
			    TRTTI_ANNOTATED_VALUE_GET(CaseNameWrapper, case_name_wrapper);

			return new_json_iterate_result_ok(result);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'TaggedMember': <no more info>",
		    "Error: unhandled iterate value at 'TaggedMember': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedType, parent)) {

		TaggedType* tagged_type = TRTTI_ANNOTATED_VALUE_CAST(TaggedType, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			if(tagged_type->type != TaggedTypeTypeNull) {
				// we have already initialized this

				tstr_static tag_name = tagged_type_type_as_string(tagged_type->type);

				RETURN_JSON_ITERATE_ERROR_DYNAMIC(
				    "Error: 'TaggedType' was encountered multiple times: <no more info>",
				    "Error: 'TaggedType' was encountered multiple times: " TSTR_FMT,
				    TSTR_STATIC_FMT_ARGS(tag_name));
			}
			// TODO: assert that the key is "struct"

			*tagged_type = (TaggedType){
				.type = TaggedTypeTypeStruct,
				.data = { .struct_type = { union_userdata->struct_id,
				                           .struct_value = C_ANONYMOUS_STRUCT_NULL() } }
			};
			++(union_userdata->struct_id);

			CAnonymousStruct* struct_slot = &(tagged_type->data.struct_type.struct_value);

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(CAnonymousStruct, struct_slot);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("struct"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(CAnonymousStruct, object_entry.entry.value)) {

					// note if we use allocated data and add here here, we could add the data
					// here, but we use a struct slot algorithm, so nothing to do here, we could
					// validate the finished entry if we like

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC("Error: invalid json value for type that "
				                                  "expected: 'CAnonymousStruct': <no more info>",
				                                  "Error: invalid json value for type that "
				                                  "expected: 'CAnonymousStruct': " TRTTI_FMT,
				                                  TRTTI_FMT_ARGS(object_entry.entry.value));

			} else if(tstr_is_null(object_entry.entry.key)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, the object entry key is <NULL>"),
				    .path = NULL,
				});
			}

			RETURN_JSON_ITERATE_ERROR_DYNAMIC(
			    "Error: unhandled object key at 'TaggedType': <no more info>",
			    "Error: unhandled object key at 'TaggedType': " TSTR_FMT,
			    TSTR_FMT_ARGS(*object_entry.entry.key));
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return NON empty, as this is the result of the subparser, so the return
			// value of this is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'TaggedType': <no more info>",
		    "Error: unhandled iterate value at 'TaggedType': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(CAnonymousStruct, parent)) {

		CAnonymousStruct* c_struct = TRTTI_ANNOTATED_VALUE_CAST(CAnonymousStruct, parent);

		IF_JSON_ITERATE_VALUE_IS_ARRAY_START(value) {

			// TODO: assert that the key is "members"
			// TODO: assert that its not a duplicate, e.g. that members is empty (empty array)

			c_struct->members = TVEC_EMPTY(StructMember);

			StructMemberArray* struct_member_array_slot = &(c_struct->members);

			RTTIAnnotatedValue result =
			    TRTTI_ANNOTATED_VALUE_GET(StructMemberArray, struct_member_array_slot);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("members"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(StructMemberArray, object_entry.entry.value)) {

					// note if we use allocated data and add here here, we could add the data
					// here, but we use a struct slot algorithm, so nothing to do here, we could
					// validate the finished entry if we like

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC("Error: invalid json value for type that "
				                                  "expected: 'StructMemberArray': <no more info>",
				                                  "Error: invalid json value for type that "
				                                  "expected: 'StructMemberArray': " TRTTI_FMT,
				                                  TRTTI_FMT_ARGS(object_entry.entry.value));

			} else if(tstr_is_null(object_entry.entry.key)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, the object entry key is <NULL>"),
				    .path = NULL,
				});
			}

			RETURN_JSON_ITERATE_ERROR_DYNAMIC(
			    "Error: unhandled object key at 'CAnonymousStruct': <no more info>",
			    "Error: unhandled object key at 'CAnonymousStruct': " TSTR_FMT,
			    TSTR_FMT_ARGS(*object_entry.entry.key));
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return NON empty, as this is the result of the subparser, so the return
			// value of this is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'CAnonymousStruct': <no more info>",
		    "Error: unhandled iterate value at 'CAnonymousStruct': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(StructMemberArray, parent)) {

		StructMemberArray* array = TRTTI_ANNOTATED_VALUE_CAST(StructMemberArray, parent);

		IF_JSON_ITERATE_VALUE_IS_OBJECT_START(value) {

			StructMember* member_slot = TVEC_PUSH_SLOT(StructMember, array);

			if(member_slot == NULL) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*member_slot = STRUCT_MEMBER_NULL();

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(StructMember, member_slot);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_IGN(value) {

			// note if we use allocated data and push here, we could push the data here, but we
			// use a push slot algorithm, so nothing to do here, we could validate the finished
			// entry if we like

			RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

			return new_json_iterate_result_ok(empty);
		}

		IF_JSON_ITERATE_VALUE_IS_ARRAY_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return NON empty, as this is the result of the subparser, so the return
			// value of this is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'StructMemberArray': <no more info>",
		    "Error: unhandled iterate value at 'StructMemberArray': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	if(TRTTI_ANNOTATED_VALUE_IS(StructMember, parent)) {

		StructMember* member = TRTTI_ANNOTATED_VALUE_CAST(StructMember, parent);

		IF_JSON_ITERATE_VALUE_IS_STRING_CONST(value) {

			tstr str_value = json_string_get_as_str(string);

			if(tstr_is_null(&str_value)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("error in getting tstr from json string"),
				    .path = NULL,
				});
			}

			tstr* str_wrapper = TJSON_NEW(tstr);

			if(str_wrapper == NULL) {
				tstr_free(&str_value);
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("OOM"),
				    .path = NULL,
				});
			}

			*str_wrapper = str_value;

			RTTIAnnotatedValue result = TRTTI_ANNOTATED_VALUE_GET(tstr, str_wrapper);

			return new_json_iterate_result_ok(result);
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(value) {

			if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("typeName"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(tstr, object_entry.entry.value)) {

					tstr* string = TRTTI_ANNOTATED_VALUE_CAST(tstr, object_entry.entry.value);
					if(string == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content"),
						    .path = NULL,
						});
					}

					if(!tstr_is_null(&(member->type_name))) {

						tstr_free(string);
						TJSON_FREE(string);
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT("implementation error, set type_name twice"),
						    .path = NULL,
						});
					}

					member->type_name = *string;
					TJSON_FREE(string);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC(
				    "Error: invalid json value for type that expected: 'tstr': <no more info>",
				    "Error: invalid json value for type that expected: 'tstr': " TRTTI_FMT,
				    TRTTI_FMT_ARGS(object_entry.entry.value));

			} else if(tstr_eq_static_tstr(object_entry.entry.key, TSTR_STATIC_LIT("name"))) {

				if(TRTTI_ANNOTATED_VALUE_IS(tstr, object_entry.entry.value)) {

					tstr* string = TRTTI_ANNOTATED_VALUE_CAST(tstr, object_entry.entry.value);
					if(string == NULL) {
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT(
						        "implementation error, returned NULL pointer but with content"),
						    .path = NULL,
						});
					}

					if(!tstr_is_null(&(member->name))) {

						tstr_free(string);
						TJSON_FREE(string);
						return new_json_iterate_result_error((JsonIterateError){
						    .err = TSTR_LIT("implementation error, set name twice"),
						    .path = NULL,
						});
					}

					member->name = *string;
					TJSON_FREE(string);

					RTTIAnnotatedValue empty = TRTTI_ANNOTATED_VALUE_GET_EMPTY();

					return new_json_iterate_result_ok(empty);
				}

				RETURN_JSON_ITERATE_ERROR_DYNAMIC(
				    "Error: invalid json value for type that expected: 'tstr': <no more info>",
				    "Error: invalid json value for type that expected: 'tstr': " TRTTI_FMT,
				    TRTTI_FMT_ARGS(object_entry.entry.value));
			} else if(tstr_is_null(object_entry.entry.key)) {
				return new_json_iterate_result_error((JsonIterateError){
				    .err = TSTR_LIT("implementation error, the object entry key is <NULL>"),
				    .path = NULL,
				});
			}

			RETURN_JSON_ITERATE_ERROR_DYNAMIC(
			    "Error: unhandled object key at 'StructMember': <no more info>",
			    "Error: unhandled object key at 'StructMember': " TSTR_FMT,
			    TSTR_FMT_ARGS(*object_entry.entry.key));
		}

		IF_JSON_ITERATE_VALUE_IS_OBJECT_END(value) {

			// NOTE: here we could check some properties of the final result, alias if the type
			// has all fields set to a valid value

			// NOTE: return NON empty, as this is the toplevel/sub parser, so the return value
			// of this is the actual result of the json_iterate function!

			return new_json_iterate_result_ok(parent);
		}

		tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

		RETURN_JSON_ITERATE_ERROR_DYNAMIC(
		    "Error: unhandled iterate value at 'StructMember': <no more info>",
		    "Error: unhandled iterate value at 'StructMember': " TSTR_FMT,
		    TSTR_STATIC_FMT_ARGS(tag_name));
	}

	tstr_static tag_name = get_current_tag_name_for_json_iterate_value(value);

	RETURN_JSON_ITERATE_ERROR_DYNAMIC("Error: unhandled iterate value: <no more info>",
	                                  "Error: unhandled iterate value: " TSTR_FMT
	                                  " on rtti: " TRTTI_FMT,
	                                  TSTR_STATIC_FMT_ARGS(tag_name), TRTTI_FMT_ARGS(parent));
}

static void free_tagged_type(TaggedType* tagged_type);

static void free_tagged_member(TaggedMember* tagged_member);

static void free_tagged_member_array(TaggedMemberArray* tagged_member_array);

static void json_value_iterator_free_tagged_unions(RTTIAnnotatedValue data) {

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnionArray, data)) {

		TaggedUnionArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnionArray, data);

		free_tagged_union_array(array);
		// TODO: is that really true now
		//  no free of the pointer as it is a address to the stack of the parent function

		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedUnion, data)) {

		TaggedUnion* tagged_union = TRTTI_ANNOTATED_VALUE_CAST(TaggedUnion, data);

		free_tagged_union(tagged_union);
		// no free of the pointer as it is a push slot of the array
		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedMemberArray, data)) {

		TaggedMemberArray* array = TRTTI_ANNOTATED_VALUE_CAST(TaggedMemberArray, data);

		free_tagged_member_array(array);
		// no free of the pointer as it is a push slot of the struct

		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedMember, data)) {

		TaggedMember* tagged_member = TRTTI_ANNOTATED_VALUE_CAST(TaggedMember, data);

		free_tagged_member(tagged_member);
		// no free of the pointer as it is a push slot of the array
		return;
	}

	if(TRTTI_ANNOTATED_VALUE_IS(TaggedType, data)) {

		TaggedType* tagged_type = TRTTI_ANNOTATED_VALUE_CAST(TaggedType, data);

		free_tagged_type(tagged_type);
		// no free of the pointer as it is a push slot of the struct
		return;
	}

	// TODO: CAnonymousStruct
	// TODO: StructMember
	// TODO: StructMemberArray

	color_fprintf(
	    stderr, COLOR_RED "ERROR: don't know how to free: " COLOR_YELLOW TRTTI_FMT "\n" COLOR_RESET,
	    TRTTI_FMT_ARGS(data));
}

NODISCARD static bool file_exists(const tstr* const input_file) {

	int fd = open(tstr_cstr(input_file), O_RDONLY);

	if(fd < 0) {
		errno = 0;
		return false;
	}

	close(fd);
	return true;
}

NODISCARD tstr_static get_tagged_unions(const tstr* const input_file, TaggedUnionArray* array) {

	if(!file_exists(input_file)) {
		color_fprintf(stderr,
		              COLOR_RED "File does not exist: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(*input_file));
		return TSTR_STATIC_LIT("file does not exist");
	}

	JsonParseResult parse_result = json_value_parse_from_file(input_file);

	IF_JSON_PARSE_RESULT_IS_ERROR_CONST(parse_result) {

		tstr error_str = json_format_error(error);

		if(tstr_is_null(&error_str)) {
			color_fprintf(stderr, COLOR_RED "File parse error: <unknown>\n" COLOR_RESET);
			return TSTR_STATIC_LIT("file parse error");
		}

		color_fprintf(stderr, COLOR_RED "File parse error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(error_str));
		tstr_free(&error_str);
		return TSTR_STATIC_LIT("file parse error");
	}

	JsonValue value = json_parse_result_get_as_ok(parse_result);

#define FREE_AT_END() \
	do { \
		free_json_value(&value); \
	} while(false)

	JsonSchema schema = get_json_schema_for_input();

	tstr json_schema_error = json_schema_validate_data(&schema, &value);

	free_json_schema(&schema);

	if(!tstr_is_null(&json_schema_error)) {

		color_fprintf(stderr, COLOR_RED "Schema error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(json_schema_error));
		tstr_free(&json_schema_error);
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file not correct schema");
	}

	*array = TVEC_EMPTY(TaggedUnion);

	RTTIAnnotatedValue array_value = TRTTI_ANNOTATED_VALUE_GET(TaggedUnionArray, array);

	TaggedUnionProcessUserdata userdata = {
		.struct_id = 0,
	};

	RTTIAnnotatedValue userdata_value =
	    TRTTI_ANNOTATED_VALUE_GET(TaggedUnionProcessUserdata, &userdata);

	JsonIterateResult iterate_res =
	    json_value_iterate(&value, json_value_iterator_process_tagged_unions,
	                       json_value_iterator_free_tagged_unions, array_value, userdata_value);

	IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF(&iterate_res) {

		tstr err_str = json_iterate_error_to_str(&(error->error));

		if(tstr_is_null(&err_str)) {
			color_fprintf(stderr,
			              COLOR_RED "Json data get error: " COLOR_YELLOW "<unknown>\n" COLOR_RESET);

			free_json_iterate_error(&(error->error));
			FREE_AT_END();
			return TSTR_STATIC_LIT("input file getting after schema check failed");
		}

		color_fprintf(stderr,
		              COLOR_RED "Json data get error: " COLOR_YELLOW TSTR_FMT "\n" COLOR_RESET,
		              TSTR_FMT_ARGS(err_str));

		tstr_free(&err_str);
		free_json_iterate_error(&(error->error));
		FREE_AT_END();
		return TSTR_STATIC_LIT("input file getting after schema check failed");
	}

	free_json_value(&value);

	return TSTR_STATIC_LIT("TODO");
}

#undef FREE_AT_END

static void free_tagged_type(TaggedType* tagged_type) {
	// TODO
	UNUSED(tagged_type);
}

static void free_tagged_member(TaggedMember* tagged_member) {
	// TODO
	UNUSED(tagged_member);
}

static void free_tagged_member_array(TaggedMemberArray* tagged_member_array) {
	// TODO
	UNUSED(tagged_member_array);
}

void free_tagged_union(TaggedUnion* tagged_union) {
	// TODO
	UNUSED(tagged_union);
}

void free_tagged_union_array(TaggedUnionArray* array) {

	for(size_t i = 0; i < TVEC_LENGTH(TaggedUnion, *array); ++i) {
		TaggedUnion* value = TVEC_GET_AT_MUT(TaggedUnion, array, i);

		free_tagged_union(value);
	}

	TVEC_FREE(TaggedUnion, array);
}
