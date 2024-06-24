
set(STD_MODULES_BUILD_TIMESTAMP_FILE std_modules_build_timestamp)

add_custom_target(m_vector ALL
    DEPENDS ${STD_MODULES_BUILD_TIMESTAMP_FILE} gcm.cache/vector.gcm
)

set(STD_MODULES_OPTIONS -fmodules-ts  -std=c++20 -c -x c++-system-header )

add_custom_command(
    OUTPUT ${STD_MODULES_BUILD_TIMESTAMP_FILE} gcm.cache/vector.gcm
    COMMAND cmake -E echo "Building vector module..."
    COMMAND ${CMAKE_CXX_COMPILER} ${STD_MODULES_OPTIONS} vector 
    COMMAND cmake -E touch ${STD_MODULES_BUILD_TIMESTAMP_FILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    BYPRODUCTS
)

