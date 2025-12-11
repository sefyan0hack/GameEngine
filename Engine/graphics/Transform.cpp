#include "Transform.hpp"
#include <core/Log.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform()
    : position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
{ CTOR_LOG }
Transform::Transform(glm::vec3 position, glm::vec3 rotaion, glm::vec3 scale)
    : position(position)
    , rotation(rotaion)
    , scale(scale)
{ CTOR_LOG }

auto Transform::operator=(const glm::mat4 &M) -> Transform
{
    return from_mat4(M);
}

Transform::operator glm::mat4 () const
{
    return to_mat4(*this);
}

auto Transform::to_mat4(const Transform& t) -> glm::mat4
{
    glm::mat4 M = glm::mat4(1.0f);

    // translate
    M = glm::translate(M, t.position);

    // rotate: build quaternion from Euler angles
    glm::quat q = glm::quat(glm::radians(t.rotation));
    M *= glm::toMat4(q);

    // scale
    M = glm::scale(M, t.scale);

    return M;
}

auto Transform::from_mat4(const glm::mat4& M) -> Transform
{  
    Transform t;

    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat orientation;

    // glm::decompose expects row-major matrix; GLM uses column-major by default,
    // so we pass M directly and it does the right thing.
    glm::decompose(
        M,
        t.scale,
        orientation,
        t.position,
        skew,
        perspective
    );

    // convert quaternion back to Euler angles (in radians), then to degrees
    glm::vec3 eulerRads = glm::eulerAngles(orientation);
    t.rotation = glm::degrees(eulerRads);

    return t;
}