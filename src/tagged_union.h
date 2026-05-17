#pragma once

#include <tstr.h>
#include <tvec.h>

#include "./utils.h"

#include "./case_name.h"

typedef struct __attribute__((packed)) {
	bool _ignore;
} Tag;

#define DEFINE_TAGGED_NAME(Name, name_member) \
	typedef struct { \
		CaseName* name; \
		Tag __impl##name_member##_member; \
	} Name;

DEFINE_TAGGED_NAME(UnionName, union)

DEFINE_TAGGED_NAME(MemberName, member)

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

DEFINE_TAGGED_NAME(RawStructName, raw_struct)

typedef struct {
	bool tag_as_error_variant;
} CppFeatures;

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

typedef struct {
	RawStructName raw_struct;
	StructOrder struct_order;
	TaggedUnionRequirements requirements;
	CppFeatures cpp_features;
} TaggedUnionOptions;

typedef struct {
	UnionName name;
	TaggedMemberArray member;
	TaggedUnionEnum enum_props;
	TaggedUnionOptions options;
} TaggedUnion;

TVEC_DEFINE_VEC_TYPE(TaggedUnion)

typedef TVEC_TYPENAME(TaggedUnion) TaggedUnionArray;

NODISCARD tstr_static get_tagged_unions(const tstr* input, TaggedUnionArray* array);

void free_tagged_union_array(TaggedUnionArray* array);
