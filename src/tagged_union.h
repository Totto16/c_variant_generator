#pragma once

#include <tstr.h>
#include <tvec.h>

#include "./utils.h"

#include "./case_name.h"

typedef struct __attribute__((packed)) {
	bool _ignore;
} Tag;

#define TAGE_NAME_IMPL(name_member) __impl##name_member##_member

#define DEFINE_TAGGED_NAME(Name, name_member) \
	typedef struct { \
		CaseName* name; \
		Tag TAGE_NAME_IMPL(name_member); \
	} Name;

DEFINE_TAGGED_NAME(UnionName, union)

DEFINE_TAGGED_NAME(MemberName, member)

#define TAGGED_NAME_NULL(Name, name_member) \
	((Name){ .name = NULL, .TAGE_NAME_IMPL(name_member) = {} })

typedef struct {
	tstr name;
} TaggedTypeSimple;

typedef struct {
	tstr type_name;
	tstr name;
} StructMember;

TVEC_DEFINE_VEC_TYPE(StructMember)

typedef TVEC_TYPENAME(StructMember) StructMemberArray;

typedef struct {
	StructMemberArray members;
} CAnonymousStruct;

typedef struct {
	size_t id;
	CAnonymousStruct struct_value;
} TaggedTypeStruct;

/**
 * @enum value
 */
typedef enum C_23_NARROW_ENUM_TO(uint8_t) {
	TaggedTypeTypeNull = 0,
	TaggedTypeTypeSimple,
	TaggedTypeTypeStruct,
} TaggedTypeType;

// manual "variant", but as this project generates them it is required at least atm,
// TODO: later use v1 to generate a bootstrap file for this variant!
typedef struct {
	TaggedTypeType type;
	union {
		TaggedTypeSimple simple_type;
		TaggedTypeStruct struct_type;
	} data;
} TaggedType;

typedef struct {
	MemberName name;
	TaggedType type;
} TaggedMember;

TVEC_DEFINE_VEC_TYPE(TaggedMember)

typedef TVEC_TYPENAME(TaggedMember) TaggedMemberArray;

DEFINE_TAGGED_NAME(EnumName, enum)

/**
 * @enum value
 */
typedef enum C_23_NARROW_ENUM_TO(uint8_t) {
	CEnumTypeBool = 0,
	CEnumTypeU8,
	CEnumTypeU16,
	CEnumTypeU32,
	CEnumTypeU64,
	CEnumTypeEndNormal,
	//
	CEnumTypeBestMatch = 0x10,
} CEnumType;

typedef struct {
	EnumName name;
	CEnumType underlying_type;
} TaggedUnionEnum;

#define TAGGED_UNION_ENUM_NULL() \
	((TaggedUnionEnum){ \
	    .name = TAGGED_NAME_NULL(EnumName, enum), \
	    .underlying_type = CEnumTypeBestMatch, \
	})

DEFINE_TAGGED_NAME(RawStructName, raw_struct)

typedef struct {
	bool tag_as_error_variant;
} CppFeatures;

#define CPP_FEATURES_NULL() \
	((CppFeatures){ \
	    .tag_as_error_variant = false, \
	})

/**
 * @enum value
 */
typedef enum C_23_NARROW_ENUM_TO(uint8_t) {
	StructOrderTagFirst = 0,
	StructOrderTagSecond,
	StructOrderEndNormal,
	//
	StructOrderAuto = 0x10,
} StructOrder;

/**
 * @enum value
 */
typedef enum C_23_NARROW_ENUM_TO(uint8_t) {
	StructOrderRequirementBestSize,
	StructOrderRequirementAlignedAccess,
} StructOrderRequirement;

typedef struct {
	StructOrderRequirement order;
} TaggedUnionRequirements;

#define TAGGED_UNION_REQUIREMENTS_NULL() \
	((TaggedUnionRequirements){ \
	    .order = StructOrderRequirementBestSize, \
	})

typedef struct {
	RawStructName raw_struct;
	StructOrder struct_order;
	TaggedUnionRequirements requirements;
	CppFeatures cpp_features;
} TaggedUnionOptions;

#define TAGGED_UNION_OPTIONS_NULL() \
	((TaggedUnionOptions){ \
	    .raw_struct = TAGGED_NAME_NULL(RawStructName, raw_struct), \
	    .struct_order = StructOrderAuto, \
	    .requirements = TAGGED_UNION_REQUIREMENTS_NULL(), \
	    .cpp_features = CPP_FEATURES_NULL(), \
	})

typedef struct {
	UnionName name;
	TaggedMemberArray member;
	TaggedUnionEnum enum_props;
	TaggedUnionOptions options;
} TaggedUnion;

#define TAGGED_UNION_NULL() \
	((TaggedUnion){ \
	    .name = TAGGED_NAME_NULL(UnionName, union), \
	    .member = TVEC_EMPTY(TaggedMember), \
	    .enum_props = TAGGED_UNION_ENUM_NULL(), \
	    .options = TAGGED_UNION_OPTIONS_NULL(), \
	})

TVEC_DEFINE_VEC_TYPE(TaggedUnion)

typedef TVEC_TYPENAME(TaggedUnion) TaggedUnionArray;

NODISCARD tstr_static get_tagged_unions(const tstr* input, TaggedUnionArray* array);

void free_tagged_union(TaggedUnion* tagged_union);

void free_tagged_union_array(TaggedUnionArray* array);
