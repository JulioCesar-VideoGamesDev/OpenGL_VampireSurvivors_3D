#include "base_math.h"
#include <random>

fn _F32::random(f32 a, f32 b) -> f32 {
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<f32> dx(a, b);
    return dx(rng); 
}

// =========================================
// @Region: Vector2
// =========================================

fn Vec2::dot(const Vec2& a, const Vec2& b) -> f32 {
    return a.x * b.x + a.y * b.y;
}

fn Vec2::distance(const Vec2& a, const Vec2& b) -> f32 {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return (f32) sqrt(dx * dx + dy * dy);
}

fn Vec2::sqrt_distance(const Vec2& a, const Vec2& b) -> f32 {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return dx * dx + dy * dy;
}

fn Vec2::random(Vec2 a, Vec2 b) -> Vec2 {
    static std::mt19937 rng{std::random_device{}()};

    f32 min_x = std::min(a.x, b.x);
    f32 max_x = std::max(a.x, b.x);
    f32 min_y = std::min(a.y, b.y);
    f32 max_y = std::max(a.y, b.y);

    std::uniform_real_distribution<f32> dx(min_x, max_x);
    std::uniform_real_distribution<f32> dy(min_y, max_y);

    return { dx(rng), dy(rng) };
}

fn Vec2::operator==(const Vec2& b) const -> bool {
    return (abs(x - b.x) <= F32.Epsilon)
        && (abs(y - b.y) <= F32.Epsilon);
}

fn Vec2::operator!=(const Vec2& b) const -> bool {
    return !(operator==(b));
}

fn Vec2::operator+(const Vec2& b) const -> Vec2 {
    return { x + b.x, y + b.y};
}

fn Vec2::operator-(const Vec2& b) const -> Vec2 {
    return { x - b.x, y - b.y };
}

fn Vec2::operator+=(const Vec2& b) -> Vec2& {
    x += b.x; y += b.y; return *this;
}

fn Vec2::operator-=(const Vec2& b) -> Vec2& {
    x -= b.x; y -= b.y; return *this;
}

fn Vec2::operator*(const Vec2& b) const -> Vec2 {
    return { x * b.x, y * b.y };
}

fn Vec2::operator/(const Vec2& b) const -> Vec2 {
    return { x / b.x, y / b.y };
}

fn Vec2::operator*=(const Vec2& b) -> Vec2& {
    x *= b.x; y *= b.y; return *this;
}

fn Vec2::operator/=(const Vec2& b) -> Vec2& {
    x /= b.x; y /= b.y; return *this;
}

fn Vec2::operator*(f32 b) const -> Vec2 {
    return { x * b, y * b };
}

fn Vec2::operator/(f32 b) const -> Vec2 {
    return { x / b, y / b };
}

fn Vec2::operator*=(f32 b) -> Vec2& {
    x *= b; y *= b; return *this;
}

fn Vec2::operator/=(f32 b) -> Vec2& {
    x /= b; y /= b; return *this;
}

fn Vec2::lenght() const -> f32 {
    return (f32) sqrt(x * x + y * y);
}

fn Vec2::normalized() const -> Vec2 {
    fn len = lenght();
    return { x / len, y / len };
}

fn Vec2::radians() const -> Vec2 {
    return { F32.to_radians(x), F32.to_radians(y) };
}

fn Vec2::degrees() const -> Vec2 {
    return { F32.to_degrees(x), F32.to_degrees(y) };
}

// =========================================
// @Region: Vector3
// =========================================

fn Vec3::dot(const Vec3& a, const Vec3& b) -> f32 {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

fn Vec3::cross(const Vec3& a, const Vec3& b) -> Vec3 {
    return 
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

fn Vec3::distance(const Vec3& a, const Vec3& b) -> f32 {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    f32 dz = a.z - b.z;
    return (f32) sqrt(dx * dx + dy * dy + dz * dz);
}

fn Vec3::sqrt_distance(const Vec3& a, const Vec3& b) -> f32 {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    f32 dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

fn Vec3::random(Vec3 a, Vec3 b) -> Vec3 {
    static std::mt19937 rng{std::random_device{}()};

    f32 min_x = std::min(a.x, b.x);
    f32 max_x = std::max(a.x, b.x);
    f32 min_y = std::min(a.y, b.y);
    f32 max_y = std::max(a.y, b.y);
    f32 min_z = std::min(a.z, b.z);
    f32 max_z = std::max(a.z, b.z);

    std::uniform_real_distribution<f32> dx(min_x, max_x);
    std::uniform_real_distribution<f32> dy(min_y, max_y);
    std::uniform_real_distribution<f32> dz(min_z, max_z);

    return { dx(rng), dy(rng), dz(rng) };
}

fn Vec3::operator==(const Vec3& b) const -> bool {
    return (abs(x - b.x) <= F32.Epsilon) 
        && (abs(y - b.y) <= F32.Epsilon) 
        && (abs(z - b.z) <= F32.Epsilon);
}

fn Vec3::operator!=(const Vec3& b) const -> bool {
    return !(operator==(b));
}

fn Vec3::operator+(const Vec3& b) const -> Vec3 {
    return { x + b.x, y + b.y, z + b.z };
}

fn Vec3::operator-(const Vec3& b) const -> Vec3 {
    return { x - b.x, y - b.y, z - b.z };
}

fn Vec3::operator+=(const Vec3& b) -> Vec3& {
    x += b.x; y += b.y; z += b.z; return *this;
}

fn Vec3::operator-=(const Vec3& b) -> Vec3& {
    x -= b.x; y -= b.y; z -= b.z; return *this;
}

fn Vec3::operator*(const Vec3& b) const -> Vec3 {
    return { x * b.x, y * b.y, z * b.z };
}

fn Vec3::operator/(const Vec3& b) const -> Vec3 {
    return { x / b.x, y / b.y, z / b.z };
}

fn Vec3::operator*=(const Vec3& b) -> Vec3& {
    x *= b.x; y *= b.y; z *= b.z; return *this;
}

fn Vec3::operator/=(const Vec3& b) -> Vec3& {
    x /= b.x; y /= b.y; z /= b.z; return *this;
}

fn Vec3::operator*(f32 b) const -> Vec3 {
    return { x * b, y * b, z * b };
}

fn Vec3::operator/(f32 b) const -> Vec3 {
    return { x / b, y / b, z / b };
}

fn Vec3::operator*=(f32 b) -> Vec3& {
    x *= b; y *= b; z *= b; return *this;
}

fn Vec3::operator/=(f32 b) -> Vec3& {
    x /= b; y /= b; z /= b; return *this;
}

fn Vec3::lenght() const -> f32 {
    return (f32) sqrt(x * x + y * y + z * z);
}

fn Vec3::normalized() const -> Vec3 {
    fn len = lenght();
    return { x / len, y / len, z / len };
}

fn Vec3::radians() const -> Vec3 {
    return { F32.to_radians(x), F32.to_radians(y), F32.to_radians(z)};
}

fn Vec3::degrees() const -> Vec3 {
    return { F32.to_degrees(x), F32.to_degrees(y), F32.to_degrees(z)};
}

// =========================================
// @Region: Vector4
// =========================================

fn Vec4::dot(const Vec4& a, const Vec4& b) -> f32 {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

fn Vec4::random(Vec4 a, Vec4 b) -> Vec4 {
    static std::mt19937 rng{std::random_device{}()};

    f32 min_r = std::min(a.x, b.x);
    f32 max_r = std::max(a.x, b.x);
    f32 min_g = std::min(a.y, b.y);
    f32 max_g = std::max(a.y, b.y);
    f32 min_b = std::min(a.z, b.z);
    f32 max_b = std::max(a.z, b.z);
    f32 min_a = std::min(a.w, b.w);
    f32 max_a = std::max(a.w, b.w);

    std::uniform_real_distribution<f32> dr(min_r, max_r);
    std::uniform_real_distribution<f32> dg(min_g, max_g);
    std::uniform_real_distribution<f32> db(min_b, max_b);
    std::uniform_real_distribution<f32> da(min_a, max_a);

    return {
        dr(rng),
        dg(rng),
        db(rng),
        da(rng)
    };
}

fn Vec4::operator==(const Vec4& b) const -> bool {
    return (abs(x - b.x) <= F32.Epsilon) 
        && (abs(y - b.y) <= F32.Epsilon) 
        && (abs(z - b.z) <= F32.Epsilon)
        && (abs(w - b.w) <= F32.Epsilon);
}

fn Vec4::operator!=(const Vec4& b) const -> bool {
    return !(operator==(b));
}

fn Vec4::operator+(const Vec4& b) const -> Vec4 {
    return { x + b.x, y + b.y, z + b.z, w + b.w };
}

fn Vec4::operator-(const Vec4& b) const -> Vec4
{
    return { x - b.x, y - b.y, z - b.z, w - b.w };
}

fn Vec4::operator+=(const Vec4& b) -> Vec4& {
    x += b.x; y += b.y; z += b.z; w += b.w;  return *this;
}

fn Vec4::operator-=(const Vec4& b) -> Vec4& {
    x -= b.x; y -= b.y; z -= b.z; w -= b.w;  return *this;
}

fn Vec4::operator*(const Vec4& b) const -> Vec4 {
    return { x * b.x, y * b.y, z * b.z, w * b.w };
}

fn Vec4::operator*(const Mat4& m) const -> Vec4 {
    return {
        x * m._11 + y * m._21 + z * m._31 + w * m._41,
        x * m._12 + y * m._22 + z * m._32 + w * m._42,
        x * m._13 + y * m._23 + z * m._33 + w * m._43,
        x * m._14 + y * m._24 + z * m._34 + w * m._44
    };
}

fn Vec4::operator/(const Vec4& b) const -> Vec4 {
    return { x / b.x, y / b.y, z / b.z, w / b.w };
}

fn Vec4::operator*=(const Vec4& b) -> Vec4& {
    x *= b.x; y *= b.y; z *= b.z; w *= b.w; return *this;
}

fn Vec4::operator/=(const Vec4& b) -> Vec4& {
    x /= b.x; y /= b.y; z /= b.z; w /= b.w; return *this;
}

fn Vec4::lenght() const -> f32 {
    return (f32) sqrt(x * x + y * y + z * z + w * w);
}

fn Vec4::normalized() const -> Vec4 {
    fn len = lenght();
    return { x / len, y / len, z / len, w / len };
}

// =========================================
// @Region: Matrix4
// =========================================

fn Mat4::from_quaternion(const Quat& q) -> Mat4 {
    Quat n = q.normalized();
    
    f32 xx = n.x * n.x;
    f32 yy = n.y * n.y;
    f32 zz = n.z * n.z;

    f32 xy = n.x * n.y;
    f32 xz = n.x * n.z;
    f32 yz = n.y * n.z;
    
    f32 wx = n.w * n.x;
    f32 wy = n.w * n.y;
    f32 wz = n.w * n.z;
    
    Mat4 m = Mat.Identity4;

    m._11 = 1.f - 2.f*(yy + zz);
    m._12 =       2.f*(xy - wz);
    m._13 =       2.f*(xz + wy);

    m._21 =       2.f*(xy + wz);
    m._22 = 1.f - 2.f*(xx + zz);
    m._23 =       2.f*(yz - wx);

    m._31 =       2.f*(xz - wy);
    m._32 =       2.f*(yz + wx);
    m._33 = 1.f - 2.f*(xx + yy);
    
    return m;
}

fn Mat4::translate(const Vec3& t) -> Mat4 {
    Mat4 m(Mat.Identity4);
    m._14 = t.x;
    m._24 = t.y;
    m._34 = t.z;
    return m;
}

fn Mat4::scale(const Vec3& s) -> Mat4 {
    Mat4 m(Mat.Identity4);
    m._11 = s.x;
    m._22 = s.y;
    m._33 = s.z;
    return m;
}

fn Mat4::rotate(const Quat& r) -> Mat4 {
    Mat4 m = from_quaternion(r);
    return m;
}

fn Mat4::rotate(const Vec3& r) -> Mat4 {
    Quat q = Quat::from_euler_angles(r);
    return rotate(q);
}

fn Mat4::transform(const Vec3& t, const Vec3& r, const Vec3& s) -> Mat4 {
    // @Note: It applies from right to left: Local scale, local rotation, world translation.
    return translate(t) * rotate(r) * scale(s);
}

fn Mat4::transpose(const Mat4& a) -> Mat4 {
    return {
        a._11,a._21,a._31,a._41,
        a._12,a._22,a._32,a._42,
        a._13,a._23,a._33,a._43,
        a._14,a._24,a._34,a._44
    };
}

fn Mat4::inverse(const Mat4& a) -> Mat4 {
    Mat4 r(Mat.Identity4);

    f32 a0 = a._11 * a._22 - a._12 * a._21;
    f32 a1 = a._11 * a._23 - a._13 * a._21;
    f32 a2 = a._11 * a._24 - a._14 * a._21;
    f32 a3 = a._12 * a._23 - a._13 * a._22;
    f32 a4 = a._12 * a._24 - a._14 * a._22;
    f32 a5 = a._13 * a._24 - a._14 * a._23;

    f32 b0 = a._31 * a._42 - a._32 * a._41;
    f32 b1 = a._31 * a._43 - a._33 * a._41;
    f32 b2 = a._31 * a._44 - a._34 * a._41;
    f32 b3 = a._32 * a._43 - a._33 * a._42;
    f32 b4 = a._32 * a._44 - a._34 * a._42;
    f32 b5 = a._33 * a._44 - a._34 * a._43;

    f32 det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    if (fabsf(det) < 1e-8f) {
        return r;
    }

    f32 inv_det = 1.0f / det;

    r._11 = ( a._22 * b5 - a._23 * b4 + a._24 * b3) * inv_det;
    r._12 = (-a._12 * b5 + a._13 * b4 - a._14 * b3) * inv_det;
    r._13 = ( a._42 * a5 - a._43 * a4 + a._44 * a3) * inv_det;
    r._14 = (-a._32 * a5 + a._33 * a4 - a._34 * a3) * inv_det;

    r._21 = (-a._21 * b5 + a._23 * b2 - a._24 * b1) * inv_det;
    r._22 = ( a._11 * b5 - a._13 * b2 + a._14 * b1) * inv_det;
    r._23 = (-a._41 * a5 + a._43 * a2 - a._44 * a1) * inv_det;
    r._24 = ( a._31 * a5 - a._33 * a2 + a._34 * a1) * inv_det;

    r._31 = ( a._21 * b4 - a._22 * b2 + a._24 * b0) * inv_det;
    r._32 = (-a._11 * b4 + a._12 * b2 - a._14 * b0) * inv_det;
    r._33 = ( a._41 * a4 - a._42 * a2 + a._44 * a0) * inv_det;
    r._34 = (-a._31 * a4 + a._32 * a2 - a._34 * a0) * inv_det;

    r._41 = (-a._21 * b3 + a._22 * b1 - a._23 * b0) * inv_det;
    r._42 = ( a._11 * b3 - a._12 * b1 + a._13 * b0) * inv_det;
    r._43 = (-a._41 * a3 + a._42 * a1 - a._43 * a0) * inv_det;
    r._44 = ( a._31 * a3 - a._32 * a1 + a._33 * a0) * inv_det;

    return r;
}

fn Mat4::determinant(const Mat4& a) -> f32 {
    f32 a0 = a._11 * a._22 - a._12 * a._21;
    f32 a1 = a._11 * a._23 - a._13 * a._21;
    f32 a2 = a._11 * a._24 - a._14 * a._21;
    f32 a3 = a._12 * a._23 - a._13 * a._22;
    f32 a4 = a._12 * a._24 - a._14 * a._22;
    f32 a5 = a._13 * a._24 - a._14 * a._23;

    f32 b0 = a._31 * a._42 - a._32 * a._41;
    f32 b1 = a._31 * a._43 - a._33 * a._41;
    f32 b2 = a._31 * a._44 - a._34 * a._41;
    f32 b3 = a._32 * a._43 - a._33 * a._42;
    f32 b4 = a._32 * a._44 - a._34 * a._42;
    f32 b5 = a._33 * a._44 - a._34 * a._43;

    return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
}

fn Mat4::view(const Vec3& p, const Quat& r) -> Mat4 {
    return inverse(rotate(r)) * inverse(translate(p));
}

fn Mat4::orthographic(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) -> Mat4 {
    Mat4 m(Mat.Identity4);

    m._11 =  2.0f / (r - l);
    m._22 =  2.0f / (t - b);
    m._33 = -2.0f / (f - n);
    
    m._14 = -(r + l) / (r - l);
    m._24 = -(t + b) / (t - b);
    m._34 = -(f + n) / (f - n); 

    return m;
}

fn Mat4::orthographic(f32 aspect, f32 size, f32 n, f32 f) -> Mat4 {
    f32 w = size * aspect;
    f32 h = size;
    return orthographic(-w, w, -h, h, n, f);
}

fn Mat4::perspective(f32 fovy, f32 aspect, f32 n, f32 f) -> Mat4 {
    Mat4 m;
    f32 t = tanf(fovy * 0.5f);

    m._11 = 1.f / (aspect * t);
    m._22 = 1.f / t;
    m._33 = - (f + n) / (f - n);
    m._43 = - 1.f;
    m._34 = - (2.f * f * n) / (f - n);
        
    return m;
}

fn Mat4::operator==(const Mat4& b) const -> bool {
    return memcmp(this, &b, sizeof(Mat4)) == 0;
}

fn Mat4::operator!=(const Mat4& b) const -> bool {
    return !(*this == b);
}

fn Mat4::operator*(const Mat4& b) const -> Mat4 {
    Mat4 r;

    r._11 = _11*b._11 + _12*b._21 + _13*b._31 + _14*b._41;
    r._21 = _21*b._11 + _22*b._21 + _23*b._31 + _24*b._41;
    r._31 = _31*b._11 + _32*b._21 + _33*b._31 + _34*b._41;
    r._41 = _41*b._11 + _42*b._21 + _43*b._31 + _44*b._41;

    r._12 = _11*b._12 + _12*b._22 + _13*b._32 + _14*b._42;
    r._22 = _21*b._12 + _22*b._22 + _23*b._32 + _24*b._42;
    r._32 = _31*b._12 + _32*b._22 + _33*b._32 + _34*b._42;
    r._42 = _41*b._12 + _42*b._22 + _43*b._32 + _44*b._42;

    r._13 = _11*b._13 + _12*b._23 + _13*b._33 + _14*b._43;
    r._23 = _21*b._13 + _22*b._23 + _23*b._33 + _24*b._43;
    r._33 = _31*b._13 + _32*b._23 + _33*b._33 + _34*b._43;
    r._43 = _41*b._13 + _42*b._23 + _43*b._33 + _44*b._43;

    r._14 = _11*b._14 + _12*b._24 + _13*b._34 + _14*b._44;
    r._24 = _21*b._14 + _22*b._24 + _23*b._34 + _24*b._44;
    r._34 = _31*b._14 + _32*b._24 + _33*b._34 + _34*b._44;
    r._44 = _41*b._14 + _42*b._24 + _43*b._34 + _44*b._44;

    return r;
}

fn Mat4::operator*=(const Mat4& b) -> Mat4& {
    *this = *this * b;
    return *this;
}

fn Mat4::operator*(const Vec4& v) const -> Vec4 {
    return {
        _11*v.x + _12*v.y + _13*v.z + _14*v.w,
        _21*v.x + _22*v.y + _23*v.z + _24*v.w,
        _31*v.x + _32*v.y + _33*v.z + _34*v.w,
        _41*v.x + _42*v.y + _43*v.z + _44*v.w
    };
}

fn Mat4::data() -> f32* {
    return &_11;
}

// =========================================
// @Region: Quaternion
// =========================================

fn Quat::from_axis_angle(const Vec3& axis, f32 rad) -> Quat {
    f32 half = rad * 0.5f;
    f32 s = sinf(half);
    
    return {
        axis.x * s,
        axis.y * s,
        axis.z * s,
        cosf(half)
    };
}

fn Quat::from_euler_angles(const Vec3& r) -> Quat {
    Quat qx = from_axis_angle(F32.Right, r.x);
    Quat qy = from_axis_angle(F32.Up, r.y);
    Quat qz = from_axis_angle(F32.Front, r.z);
    return qz * qy * qx;
}

fn Quat::operator==(const Quat& b) const -> bool {
    return (abs(x - b.x) <= F32.Epsilon) 
        && (abs(y - b.y) <= F32.Epsilon) 
        && (abs(z - b.z) <= F32.Epsilon)
        && (abs(w - b.w) <= F32.Epsilon);
}

fn Quat::operator!=(const Quat& b) const -> bool {
    return !(operator==(b));
}

fn Quat::operator*(const Quat& b) const -> Quat {
    return {
        w*b.x + x*b.w + y*b.z - z*b.y,
        w*b.y - x*b.z + y*b.w + z*b.x,
        w*b.z + x*b.y - y*b.x + z*b.w,
        w*b.w - x*b.x - y*b.y - z*b.z
    };
}

fn Quat::operator*=(const Quat& b) -> Quat& {
    return *this = *this * b;
}

fn Quat::lenght() const -> f32 {
    return (f32) sqrt(x * x + y * y + z * z + w * w);
}

fn Quat::normalized() const -> Quat {
    fn len = lenght();
    return { x / len, y / len, z / len, w / len };
}

// =========================================
// @Region: Camera
// =========================================

fn Free_Look::move_forward(f32 dt)  -> void { m_camera->pos = m_camera->pos + forward() * (speed * dt); }
fn Free_Look::move_backward(f32 dt) -> void { m_camera->pos = m_camera->pos - forward() * (speed * dt); }
fn Free_Look::move_right(f32 dt)    -> void { m_camera->pos = m_camera->pos + right()   * (speed * dt); }
fn Free_Look::move_left(f32 dt)     -> void { m_camera->pos = m_camera->pos - right()   * (speed * dt); }
fn Free_Look::move_up(f32 dt)       -> void { m_camera->pos.y += speed * dt; }
fn Free_Look::move_down(f32 dt)     -> void { m_camera->pos.y -= speed * dt; }

fn Free_Look::rotate(f32 dx, f32 dy) -> void {
    m_camera->rot.y -= dx * sensivity; 
    m_camera->rot.x -= dy * sensivity;

    if (m_camera->rot.x > CLAMP_ANGLE)  m_camera->rot.x = CLAMP_ANGLE;
    if (m_camera->rot.x < -CLAMP_ANGLE) m_camera->rot.x = -CLAMP_ANGLE;
}

fn Free_Look::forward() const -> Vec3 {
    f32 pitch = F32.to_radians(m_camera->rot.x);
    f32 yaw = F32.to_radians(m_camera->rot.y);

    return Vec3{
        (f32) -cos(pitch) * (f32) sin(yaw),
        (f32)  sin(pitch),
        (f32) -cos(pitch) * (f32) cos(yaw)
    }.normalized();
}

fn Free_Look::right() const -> Vec3 {
    return Vec3::cross(forward(), Vec3 { 0,1,0 }).normalized();
}

fn Camera::update_matrix(s32 viewport_x, s32 viewport_y) -> void {
    Mat4 p;
    const f32 aspect = (f32)viewport_x / (f32)viewport_y;

    switch (mode)
    {
    case Camera::Perspective:
        p = Mat4::perspective(F32.to_radians(fov), aspect, near_plane, far_plane);
        break;
    case Camera::Orthographic:
        p = Mat4::orthographic(aspect, size, near_plane, far_plane);
        break;
    }
    
    Mat4 v = Mat4::view(pos, Quat::from_euler_angles(rot.radians()));
    m_matrix = Mat4::transpose(p * v);
}

fn AABB::overlap(const AABB& a, const AABB& b) -> bool {
    return (abs(a.x - b.x) <= (a.half_w + b.half_w)) &&
           (abs(a.y - b.y) <= (a.half_h + b.half_h));
}