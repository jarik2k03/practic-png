#pragma once
#ifndef SHADERS_CSC_TRIANGLE_PATHS_H
#define SHADERS_CSC_TRIANGLE_PATHS_H

namespace csc {
namespace pngine {
namespace paths {

constexpr const char* bring_triangle_vert_shader_path() noexcept {
  return "${CMAKE_CURRENT_LIST_DIR}";
}

}
}
}
#endif
