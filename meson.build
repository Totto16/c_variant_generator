project(
    'c_variant_generator',
    'c',
    license: 'MIT',
    meson_version: '>=1.10.0',
    default_options: {
        'buildtype': 'release',
        'optimization': '3',
        'c_std': ['gnu23', 'gnu2x'],
        'warning_level': '3',
        'werror': 'true',
        'b_ndebug': 'if-release',
    },
    version: '2.0.0-alpha',
)

deps = []
compile_flags = []

deps += dependency(
    'tvec.h',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

deps += dependency(
    'tmap.h',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

deps += dependency(
    'tstr.h',
    allow_fallback: true,
    default_options: {
        'static_string_modification_behavior': 'error',
    },
    version: ['>=1.0.0', '<2'],
)

deps += dependency(
    'tstr_builder',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

deps += dependency(
    'tjson',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

cc = meson.get_compiler('c')

deps += cc.find_library(
    'm',
    required: false,
)

internal_c_args = []

internal_c_args += cc.get_supported_arguments(
    '-Wassign-enum',
    '-Wenum-conversion',
    '-Wenum-switch',
    '-fstrict-enums',
    '-Wenum-identifier',
    '-Wenum-compare',
    '-Wenum-enum-conversion',
    '-Wenum-float-conversion',
    '-Wenum-too-large',
    '-Wenum-compare-switch',
    '-Wenum-compare-conditional',
    '-fno-strict-aliasing',
    '-Wpointer-arith',
    '-Wmissing-declarations',
    '-Wformat=2',
    '-Wstrict-prototypes',
    '-Wmissing-prototypes',
    '-Wnested-externs',
    '-Wold-style-definition',
    '-Wundef',
    '-Wunused',
    '-Wcast-align',
    '-Wmissing-noreturn',
    '-Wmissing-format-attribute',
    '-Wmissing-include-dirs',
    '-Wlogical-op',
    '-Wignored-qualifiers',
    '-Werror=redundant-decls',
    '-Werror=implicit',
    '-Werror=nonnull',
    '-Werror=init-self',
    '-Werror=main',
    '-Werror=missing-braces',
    '-Werror=sequence-point',
    '-Werror=return-type',
    '-Werror=trigraphs',
    '-Werror=array-bounds',
    '-Werror=write-strings',
    '-Werror=address',
    '-Werror=int-to-pointer-cast',
    '-Werror=pointer-to-int-cast',
    '-Werror=empty-body',
    '-Werror=write-strings',
    '-Werror=strict-aliasing',
    '-Wsign-compare',
    '-Wno-cast-function-type',
    '-Wunused-parameter',
    '-Wmissing-field-initializers',
    '-Wtype-limits',
    '-Wshadow',
    '-Wfloat-conversion',
    '-Wno-nullability-extension',
    '-Wno-nullability-completeness',
    '-Wcast-qual',
    '-Wwrite-strings',
    '-Wdiscarded-qualifiers',
)

if cc.get_id() == 'gcc'
    internal_c_args += cc.get_supported_arguments('-Wno-format-nonliteral')

    if get_option('buildtype') == 'release'
        # wait for the fix of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=124151
        #to land in the gcc the Ci uses, after that disable that
        internal_c_args += cc.get_supported_arguments('-fno-tree-sra')
    endif

endif

src_files = []
inc_dirs = []

subdir('src')

c_variant_generator_exe = executable(
    'c_variant_generator_exe',
    src_files,
    dependencies: deps,
    include_directories: inc_dirs,
    c_args: [compile_flags, internal_c_args],
    install: true,
)

meson.override_find_program('c_variant_generator', c_variant_generator_exe)
