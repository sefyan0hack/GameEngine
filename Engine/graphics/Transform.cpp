#include "Transform.hpp"

#include <core/Log.hpp>
#include <emath/emath.hpp>

Transform::Transform()
    : position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
{}
Transform::Transform(emath::vec3 pos, emath::vec3 rot, emath::vec3 s)
    : position(pos)
    , rotation(rot)
    , scale(s)
{}

auto Transform::operator=(const emath::mat4 &M) -> Transform
{
    return from_mat4(M);
}

Transform::operator emath::mat4 () const
{
    return to_mat4(*this);
}

auto Transform::to_mat4(const Transform& t) -> emath::mat4
{
    emath::mat4 M = emath::mat4(1.0f);

    // translate
    M = emath::translate(M, t.position);

    // rotate: build quaternion from Euler angles
    emath::quat q = emath::quat(emath::to_rad(t.rotation));
    // M *= emath::to_mat4(q);
    M = M * emath::to_mat4(q);

    // scale
    M = emath::scale(M, t.scale);

    return M;
}

auto Transform::from_mat4(const emath::mat4& M) -> Transform
{  
    Transform t;

    emath::vec3 skew;
    emath::vec4 perspective;
    emath::quat orientation;

    // emath::decompose expects row-major matrix; GLM uses column-major by default,
    // so we pass M directly and it does the right thing.
    emath::decompose(
        M,
        t.scale,
        orientation,
        t.position,
        skew,
        perspective
    );

    // convert quaternion back to Euler angles (in radians), then to degrees
    emath::vec3 eulerRads = emath::euler_angles(orientation);
    t.rotation = emath::to_deg(eulerRads);

    return t;
}