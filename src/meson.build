src_files += files(
    'generate.c',
    'generate.h',
    'main.c',
    'utils.h',
)

inc_dirs += include_directories('.')

stdc_version = cc.get_define('__STDC_VERSION__ ')
stdc_version_int = stdc_version.replace('L', '').to_int()
if stdc_version_int >= 202000
    compile_flags += '-D_C_VARIANT_GENERATOR_COMPILE_WITH_NARROWED_ENUMS'
endif

