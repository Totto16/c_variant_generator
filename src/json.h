#pragma once

#include "./utils.h"

#include <tjson.h>

#include <tjson_schema.h>

NODISCARD JsonSchema get_json_schema_for_input(void);
