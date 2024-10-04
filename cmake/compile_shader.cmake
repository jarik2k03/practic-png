function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.${arg_FORMAT}
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_exe}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                -MD -MF ${source}.d
                -o ${source}.${arg_FORMAT}
                ${CMAKE_CURRENT_SOURCE_DIR}/${source}
        )
        target_sources(${target} PRIVATE ${source}.${arg_FORMAT})
    endforeach()
endfunction()

function(create_shaderpath_header target)
  add_custom_command(
    OUTPUT ${target}.h
    DEPENDS ${target}
    DEPFILE ${target}.d
  )
  string(TOUPPER ${target}.h str_signature)
  string(REPLACE "." "_" str_signature ${str_signature})

  string(APPEND str_target
    "\#pragma once\n"
    "\#ifndef " ${str_signature} "\n"
    "\#define " ${str_signature} "\n"
    "// is machine-generated header with shader-paths\n"
    "namespace csc {\n"
    "namespace pngine {\n"
    "namespace shaders {\n"
    "struct ${target} {\n"
  )

  get_target_property(prop_sources "${target}" SOURCES)
  get_target_property(prop_binary_dir "${target}" BINARY_DIR)
  foreach(compiled_source IN LISTS prop_sources)
    string(REGEX REPLACE "\\.[^.]*$" "" str_source_file_extension ${compiled_source}) # убирает расширение .bin
    string(REGEX REPLACE "^[^.]*\\." "" str_source_file_extension ${str_source_file_extension}) # оставляет только тип шейдера
    string(APPEND str_target
      "  static constexpr const char* " ${str_source_file_extension} "_path = \"${prop_binary_dir}/${compiled_source}\";\n"
    )
  endforeach()
  string(APPEND str_target
    "}; \n"
    "} // namespace shaders\n"
    "} // namespace pngine\n"
    "} // namespace csc\n"
    "\#endif\n")
  file(GENERATE OUTPUT ${target}.h CONTENT "${str_target}")
  target_sources(${target} PRIVATE ${target}.h)

endfunction()
