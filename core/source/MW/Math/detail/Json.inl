/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>

namespace mw {
inline namespace math {

inline void to_json(json& obj, const IVec2& v) {
    obj["x"] = v.x;
    obj["y"] = v.y;
}

inline void from_json(const json& obj, IVec2& v) {
    v.x = obj.value("x", v.x);
    v.y = obj.value("y", v.y);
}

inline void to_json(json& obj, const IVec4& v) {
    obj["x"] = v.x;
    obj["y"] = v.y;
    obj["z"] = v.z;
    obj["w"] = v.w;
}

inline void from_json(const json& obj, IVec4& v) {
    v.x = obj.value("x", v.x);
    v.y = obj.value("y", v.y);
    v.z = obj.value("z", v.z);
    v.w = obj.value("w", v.w);
}

inline void to_json(json& obj, const Vec2& v) {
    obj["x"] = v.x;
    obj["y"] = v.y;
}

inline void from_json(const json& obj, Vec2& v) {
    v.x = obj.value("x", v.x);
    v.y = obj.value("y", v.y);
}

inline void to_json(json& obj, const Vec3& v) {
    obj["x"] = v.x;
    obj["y"] = v.y;
    obj["z"] = v.z;
}

inline void from_json(const json& obj, Vec3& v) {
    v.x = obj.value("x", v.x);
    v.y = obj.value("y", v.y);
    v.z = obj.value("z", v.z);
}

inline void to_json(json& obj, const Vec4& v) {
    obj["x"] = v.x;
    obj["y"] = v.y;
    obj["z"] = v.z;
    obj["w"] = v.w;
}

inline void from_json(const json& obj, Vec4& v) {
    v.x = obj.value("x", v.x);
    v.y = obj.value("y", v.y);
    v.z = obj.value("z", v.z);
    v.w = obj.value("w", v.w);
}

inline void to_json(json& obj, const Mat2& m) {
    obj["m11"] = m.m11;
    obj["m12"] = m.m12;
    obj["m21"] = m.m21;
    obj["m22"] = m.m22;
}

inline void from_json(const json& obj, Mat2& m) {
    m.m11 = obj.value("m11", m.m11);
    m.m12 = obj.value("m12", m.m12);
    m.m21 = obj.value("m21", m.m21);
    m.m22 = obj.value("m22", m.m22);
}

inline void to_json(json& obj, const Mat3& m) {
    obj["m11"] = m.m11;
    obj["m12"] = m.m12;
    obj["m13"] = m.m13;
    obj["m21"] = m.m21;
    obj["m22"] = m.m22;
    obj["m23"] = m.m23;
    obj["m31"] = m.m31;
    obj["m32"] = m.m32;
    obj["m33"] = m.m33;
}

inline void from_json(const json& obj, Mat3& m) {
    m.m11 = obj.value("m11", m.m11);
    m.m12 = obj.value("m12", m.m12);
    m.m13 = obj.value("m13", m.m13);
    m.m21 = obj.value("m21", m.m21);
    m.m22 = obj.value("m22", m.m22);
    m.m23 = obj.value("m23", m.m23);
    m.m31 = obj.value("m31", m.m31);
    m.m32 = obj.value("m32", m.m32);
    m.m33 = obj.value("m33", m.m33);
}

inline void to_json(json& obj, const Mat4& m) {
    obj["m11"] = m.m11;
    obj["m12"] = m.m12;
    obj["m13"] = m.m13;
    obj["m14"] = m.m14;
    obj["m21"] = m.m21;
    obj["m22"] = m.m22;
    obj["m23"] = m.m23;
    obj["m24"] = m.m24;
    obj["m31"] = m.m31;
    obj["m32"] = m.m32;
    obj["m33"] = m.m33;
    obj["m34"] = m.m34;
    obj["m41"] = m.m41;
    obj["m42"] = m.m42;
    obj["m43"] = m.m43;
    obj["m44"] = m.m44;
}

inline void from_json(const json& obj, Mat4& m) {
    m.m11 = obj.value("m11", m.m11);
    m.m12 = obj.value("m12", m.m12);
    m.m13 = obj.value("m13", m.m13);
    m.m14 = obj.value("m14", m.m14);
    m.m21 = obj.value("m21", m.m21);
    m.m22 = obj.value("m22", m.m22);
    m.m23 = obj.value("m23", m.m23);
    m.m24 = obj.value("m24", m.m24);
    m.m31 = obj.value("m31", m.m31);
    m.m32 = obj.value("m32", m.m32);
    m.m33 = obj.value("m33", m.m33);
    m.m34 = obj.value("m34", m.m34);
    m.m41 = obj.value("m41", m.m41);
    m.m42 = obj.value("m42", m.m42);
    m.m43 = obj.value("m43", m.m43);
    m.m44 = obj.value("m44", m.m44);
}

inline void to_json(json& obj, const IntRect& r) {
    obj["x"] = r.x;
    obj["y"] = r.y;
    obj["w"] = r.w;
    obj["h"] = r.h;
}

inline void from_json(const json& obj, IntRect& r) {
    r.x = obj.value("x", r.x);
    r.y = obj.value("y", r.y);
    r.w = obj.value("w", r.w);
    r.h = obj.value("h", r.h);
}

inline void to_json(json& obj, const Plane& p) {
    obj["a"] = p.a;
    obj["b"] = p.b;
    obj["c"] = p.c;
    obj["d"] = p.d;
}

inline void from_json(const json& obj, Plane& p) {
    p.a = obj.value("a", p.a);
    p.b = obj.value("b", p.b);
    p.c = obj.value("c", p.c);
    p.d = obj.value("d", p.d);
}

inline void to_json(json& obj, const Quat& q) {
    obj["x"] = q.v.x;
    obj["y"] = q.v.y;
    obj["z"] = q.v.z;
    obj["w"] = q.w;
}

inline void from_json(const json& obj, Quat& q) {
    q.v.x = obj.value("x", q.v.x);
    q.v.y = obj.value("y", q.v.y);
    q.v.z = obj.value("z", q.v.z);
    q.w = obj.value("w", q.w);
}

inline void to_json(json& obj, const AABox& b) {
    obj["center"] = b.center;
    obj["extents"] = b.extents;
}

inline void from_json(const json& obj, AABox& b) {
    b.center = obj.value("center", b.center);
    b.extents = obj.value("extents", b.extents);
}

inline void to_json(json& obj, const Ray& r) {
    obj["origin"] = r.origin;
    obj["direction"] = r.direction;
}

inline void from_json(const json& obj, Ray& r) {
    r.origin = obj.value("origin", r.origin);
    r.direction = obj.value("direction", r.direction);
}

inline void to_json(json& obj, const Sphere& s) {
    obj["center"] = s.center;
    obj["radius"] = s.radius;
}

inline void from_json(const json& obj, Sphere& s) {
    s.center = obj.value("center", s.center);
    s.radius = obj.value("radius", s.radius);
}

inline void to_json(json& obj, const Transform& t)
{
    obj["position"] = t.position;
    obj["rotation"] = t.rotation;
    obj["scale"] = t.scale;
}

inline void from_json(const json& obj, Transform& t)
{
    t.position = obj["position"];
    t.rotation = obj["rotation"];
    t.scale = obj["scale"];
}

inline void to_json(json& obj, const Triangle& t) {
    obj["a"] = t.a;
    obj["b"] = t.b;
    obj["c"] = t.c;
}

inline void from_json(const json& obj, Triangle& t) {
    t.a = obj.value("a", t.a);
    t.b = obj.value("b", t.b);
    t.c = obj.value("c", t.c);
}

} // math
} // mw
