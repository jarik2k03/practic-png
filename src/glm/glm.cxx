module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
export module glm_hpp;

export namespace glm {
using glm::ivec2;
using glm::lookAt;
using glm::mat3x3;
using glm::mat4;
using glm::mat4x3;
using glm::ortho;
using glm::perspective;
using glm::radians;
using glm::scale;
using glm::translate;
using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::operator!=;
using glm::operator==;
using glm::operator+;
using glm::operator*;
using glm::inverse;
using glm::pi;
using glm::transpose;
} // namespace glm
