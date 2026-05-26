#pragma once

// =========================================
// @Region: Common f32 stuff.
// =========================================

#include <math.h>
#include <stdint.h>
#include <float.h>

inline fn clamp(f32 value, f32 min_value, f32 max_value) -> f32 {
    return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}

inline fn clamp(f64 value, f64 min_value, f64 max_value) -> f64 {
    return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}

struct _F32 {

    f32 Radians_In_Dregree = 0.0174533f;
    f32 Degrees_In_Radian = 57.2958f;
    f32 PI = 3.14159265358979323846f;
    f32 Epsilon = FLT_EPSILON;
    
    f32 Zero  [3] = { 0.f, 0.f, 0.f };
    f32 One   [3] = { 1.f, 1.f, 1.f };
    f32 Right [3] = { 1.f, 0.f, 0.f };
    f32 Left  [3] = {-1.f, 0.f, 0.f };
    f32 Up    [3] = { 0.f, 1.f, 0.f };
    f32 Down  [3] = { 0.f,-1.f, 0.f };
    f32 Front [3] = { 0.f, 0.f, 1.f };
    f32 Back  [3] = { 0.f, 0.f,-1.f };

    constexpr f32 to_radians(f32 degrees) const { return degrees * Radians_In_Dregree; }
    constexpr f32 to_degrees(f32 radians) const { return radians * Degrees_In_Radian; }
    static fn random(f32 a, f32 b) -> f32;
};

struct _S32 {
    s32 Max = INT32_MAX;
    s32 Min = INT32_MIN;
};

struct _U32 {
    u32 Max = UINT32_MAX;
};

inline static constexpr _F32 F32;
inline static constexpr _U32 U32;
inline static constexpr _S32 S32;

// =========================================
// @Region: Vector2
// =========================================

class Vec2 {
public:
    // @Note: Returns how much a goes in the direction of b. (0 -> perpendicular)
    static fn dot(const Vec2& a, const Vec2& b) -> f32;
    // @Note: Returns the euclidean (real) distance between to points.
    static fn distance(const Vec2& a, const Vec2& b) -> f32;
    // @Note: Same as distance but without the std::sqrt. std::sqrt is a heavy operation, this is useful for comparisons.
    static fn sqrt_distance(const Vec2& a, const Vec2& b) -> f32;
    
    static fn random(Vec2 a, Vec2 b) -> Vec2;
    
    constexpr Vec2() : x(0), y(0) {}
    constexpr Vec2(f32 x, f32 y) : x(x), y(y) {}
    constexpr Vec2(const f32 (&b)[2]) : x(b[0]), y(b[1]) {}
    constexpr Vec2(const f32 (&b)[3]) : x(b[0]), y(b[1]) {}
    
    fn operator==(const Vec2& b) const -> bool;
    fn operator!=(const Vec2& b) const -> bool;

    fn operator+(const Vec2& b) const -> Vec2;
    fn operator-(const Vec2& b) const -> Vec2;
    
    fn operator+=(const Vec2& b) -> Vec2&;
    fn operator-=(const Vec2& b) -> Vec2&;
    
    fn operator*(const Vec2& b) const -> Vec2;
    fn operator/(const Vec2& b) const -> Vec2;
    
    fn operator*=(const Vec2& b) -> Vec2&;
    fn operator/=(const Vec2& b) -> Vec2&;

    fn operator*(f32 b) const->Vec2;
    fn operator/(f32 b) const->Vec2;

    fn operator*=(f32 b) -> Vec2&;
    fn operator/=(f32 b) -> Vec2&;

    fn lenght() const -> f32;
    fn normalized() const -> Vec2;

    fn radians() const -> Vec2;
    fn degrees() const -> Vec2;
    
    f32 x, y;
};

struct Vec2i {
    s32 x = 0;
    s32 y = 0;
};

// =========================================
// @Region: Vector3
// =========================================

class Vec3 {
public:    
    // @Note: Returns how much a goes in the direction of b. (0 -> perpendicular)
    static fn dot(const Vec3& a, const Vec3& b) -> f32;
    // @Note: Returns a perpendicular vector to the (a, b) plane.
    static fn cross(const Vec3& a, const Vec3& b) -> Vec3;
    // @Note: Returns the euclidean (real) distance between to points.
    static fn distance(const Vec3& a, const Vec3& b) -> f32;
    // @Note: Same as distance but without the std::sqrt. std::sqrt is a heavy operation, this is useful for comparisons.
    static fn sqrt_distance(const Vec3& a, const Vec3& b) -> f32;
    
    static fn random(Vec3 a, Vec3 b) -> Vec3;

    constexpr Vec3() : x(0), y(0), z(0) {}
    constexpr Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}
    constexpr Vec3(const f32(&b) [2]) : x(b[0]), y(b[1]), z(0) {}
    constexpr Vec3(const f32(&b) [3]) : x(b[0]), y(b[1]), z(b[2]) {}
    constexpr Vec3(const f32(&b) [4]) : x(b[0]), y(b[1]), z(b[2]) {}

    fn operator==(const Vec3& b) const -> bool;
    fn operator!=(const Vec3& b) const -> bool;

    fn operator+(const Vec3& b) const -> Vec3;
    fn operator-(const Vec3& b) const -> Vec3;
    
    fn operator+=(const Vec3& b) -> Vec3&;
    fn operator-=(const Vec3& b) -> Vec3&;
    
    fn operator*(const Vec3& b) const -> Vec3;
    fn operator/(const Vec3& b) const -> Vec3;
    
    fn operator*=(const Vec3& b) -> Vec3&;
    fn operator/=(const Vec3& b) -> Vec3&;
    
    fn operator*(f32 b) const -> Vec3;
    fn operator/(f32 b) const -> Vec3;

    fn operator*=(f32 b) -> Vec3&;
    fn operator/=(f32 b) -> Vec3&;
    
    fn lenght() const -> f32;
    fn normalized() const -> Vec3;
    
    fn radians() const -> Vec3;
    fn degrees() const -> Vec3;

    f32 x, y, z;
};

class Mat4;

// =========================================
// @Region: Vector4
// =========================================

class Vec4 {
public:
    // @Note: Returns how much a goes in the direction of b. (0 -> perpendicular)
    static fn dot(const Vec4& a, const Vec4& b) -> f32;
    
    static fn random(Vec4 a, Vec4 b) -> Vec4;

    constexpr Vec4() : x(0), y(0), z(0), w(0) {}
    constexpr Vec4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
    constexpr Vec4(const f32(&b) [4]) : x(b[0]), y(b[1]), z(b[2]), w(b[3]) {}

    fn operator==(const Vec4& b) const -> bool;
    fn operator!=(const Vec4& b) const -> bool;

    fn operator+(const Vec4& b) const -> Vec4;
    fn operator-(const Vec4& b) const -> Vec4;
    
    fn operator+=(const Vec4& b) -> Vec4&;
    fn operator-=(const Vec4& b) -> Vec4&;
    
    fn operator*(const Vec4& b) const -> Vec4;
    fn operator*(const Mat4& m) const -> Vec4;
    fn operator/(const Vec4& b) const -> Vec4;
    
    fn operator*=(const Vec4& b) -> Vec4&;
    fn operator/=(const Vec4& b) -> Vec4&;
    
    fn lenght() const -> f32;
    fn normalized() const -> Vec4;
    
    f32 x, y, z, w;
};

struct _Color {
    f32 White            [4] = { 1.f, 1.f, 1.f, 1.f };
    f32 White_Faded      [4] = { 1.f, 1.f, 1.f, 0.f };
    f32 Black            [4] = { 0.f, 0.f, 0.f, 1.f };
    f32 Corn_Flower_Blue [4] = { .388f, .584f, .933f, 1.f };
    f32 Blue             [4] = { 0.f, 0.f, 1.f, 1.f };
    f32 Light_Blue       [4] = { .3f, .3f, 1.f, 1.f };
    f32 Cyan             [4] = { 0.f, 1.f, 1.f, 1.f };
    f32 Gray             [4] = { .5f, .5f, .5f, 1.f };
    f32 Dark_Gray        [4] = { .2f, .2f, .2f, 1.f };
    f32 Green            [4] = { 0.f, 1.f, 0.f, 1.f };
    f32 Light_Green      [4] = { .3f, 1.f, .3f, 1.f };
    f32 Chill_Green      [4] = {0.0471f, 0.6510f, 0.4078f, 1.f };
    f32 Magenta          [4] = { 1.f, 0.f, 1.f, 1.f };
    f32 Red              [4] = { 1.f, 0.f, 0.f, 1.f };
    f32 Light_Red        [4] = { 1.f, .3f, .3f, 1.f };
    f32 Yellow           [4] = { 1.f, .92f, .016f, 1.f };
    f32 Orange           [4] = { .97f, .60f, .11f, 1.f };
};

inline static constexpr _Color Color;

// =========================================
// @Region: Matrix4
// =========================================

class Vec3;
class Vec4;
class Quat;

//@Pending: 
// - Matrix4::decompose, 
// - Optimized Vector2 rotation (not using Quaternions & Z axis)
// - Matrix4::screen_to_world
// - Matrix4::world_to_screen

class Mat4 {
public:
    static fn from_quaternion(const Quat& q) -> Mat4;

    // @Note: Transformations. 
    static fn translate(const Vec3& t) -> Mat4;
    static fn scale(const Vec3& s) -> Mat4;
    static fn rotate(const Quat& r) -> Mat4;
    static fn rotate(const Vec3& r) -> Mat4;
    static fn transform(const Vec3& t, const Vec3& r = F32.Zero, const Vec3& s = F32.One) -> Mat4;

    // @Note: Swap rows & columns.
    static fn transpose(const Mat4& a) -> Mat4;
    // @Note: Reverts the operation. Ex: Prev M -> Transformed M | Prev M = Transformed M-1
    static fn inverse(const Mat4& a) -> Mat4;
    // @Note: Mesures volume's scale, check mirrored, if returns 0 inverse does not exist.
    static fn determinant(const Mat4& a) -> f32;
    
    // @Note: RH (Right Handed): Negative Z goes forward. (OpenGL)
    // NO (Negative One to One): Depth values are normalized between -1 and 1.
    // ------------------------
    static fn view(const Vec3& p, const Quat& r) -> Mat4;
    static fn orthographic(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) -> Mat4;
    static fn orthographic(f32 aspect, f32 size, f32 n, f32 f) -> Mat4;
    static fn perspective(f32 fovy, f32 aspect, f32 n, f32 f) -> Mat4;
    // ------------------------
    // @Pending: Do the LH version.

    constexpr Mat4()
        : _11(0.f), _12(0.f), _13(0.f), _14(0.f)
        , _21(0.f), _22(0.f), _23(0.f), _24(0.f)
        , _31(0.f), _32(0.f), _33(0.f), _34(0.f)
        , _41(0.f), _42(0.f), _43(0.f), _44(0.f)
    {
    }

    constexpr Mat4(
          f32 _11, f32 _12, f32 _13, f32 _14
        , f32 _21, f32 _22, f32 _23, f32 _24
        , f32 _31, f32 _32, f32 _33, f32 _34
        , f32 _41, f32 _42, f32 _43, f32 _44
    )
        : _11(_11), _12(_12), _13(_13), _14(_14)
        , _21(_21), _22(_22), _23(_23), _24(_24)
        , _31(_31), _32(_32), _33(_33), _34(_34)
        , _41(_41), _42(_42), _43(_43), _44(_44)
    {
    }

    fn operator==(const Mat4& b) const -> bool;
    fn operator!=(const Mat4& b) const -> bool;

    fn operator*(const Mat4& b) const -> Mat4;
    fn operator*=(const Mat4& b) -> Mat4&;
    
    fn operator*(const Vec4& v) const -> Vec4;

    fn data() -> f32*;
    
    // @Note: Row major matrix. First number is the row, Second is the column. (D3D11)
    f32 _11, _12, _13, _14;
    f32 _21, _22, _23, _24;
    f32 _31, _32, _33, _34;
    f32 _41, _42, _43, _44;
};

struct _Mat {
    Mat4 Zero4  = {
        0.f, 0.f, 0.f, 0.f, 
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f 
    };
    Mat4 Identity4 = {
        1.f, 0.f, 0.f, 0.f, 
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f 
    };
};

inline static constexpr _Mat Mat;

// =========================================
// @Region: Quaternion
// =========================================

class Mat4;
class Vec4;
class Vec3;

class Quat {
public:
    // @Note: Rotates n radians in axis direction.
    static fn from_axis_angle(const Vec3& axis, f32 rad) -> Quat;
    // @Note: Rotates using a classic vector (it takes radians).
    static fn from_euler_angles(const Vec3& r) -> Quat;

    constexpr Quat() : x(0), y(0), z(0), w(0) {}
    constexpr Quat(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}

    fn operator==(const Quat& b) const -> bool;
    fn operator!=(const Quat& b) const -> bool;

    fn operator*(const Quat& b) const -> Quat;
    fn operator*=(const Quat& b) -> Quat&;
    
    fn lenght() const -> f32;
    fn normalized() const -> Quat;
    
    // @Note: This represents a normalized dir (x, y, z) and an angle (w) (in radians).
    f32 x, y, z, w;
};

class Camera;

class Free_Look {
public:
    inline static constexpr f32 CLAMP_ANGLE = 89.f;

    Free_Look() = default;
    ~Free_Look() = default;
    
    fn init(Camera* camera) { m_camera = camera; }

    fn move_forward(f32 dt) -> void;
    fn move_backward(f32 dt) -> void;
    fn move_right(f32 dt) -> void;
    fn move_left(f32 dt) -> void;
    fn move_up(f32 dt) -> void;
    fn move_down(f32 dt) -> void;
    fn rotate(f32 dx, f32 dy) -> void;
    fn forward() const -> Vec3;
    fn right() const -> Vec3;
    
    f32 speed = 15.f;
    f32 sensivity = 0.1f;

private:
    Camera* m_camera = nullptr;
};

// =========================================
// @Region: Camera
// =========================================

class Camera {
public:
    enum Mode : u8 {
          None
        , Perspective
        , Orthographic
    };

    Camera(Mode mode)
        : mode(mode) 
    {
        // @Note: Default values for 3D.
        if (mode == Mode::Perspective)
        {
            pos = Vec3(F32.Front) * 40.f;
            near_plane = 0.1f;
            far_plane = 100.f;
            fov = 60;
            return;
        }

        // @Note: Default values for 2D.
        if (mode == Mode::Orthographic)
        {
            near_plane = 0.f;
            far_plane = 1.f;
            size = 3;
            return;
        }
    }

    Camera() = default;
    ~Camera() = default;

    fn matrix() const -> Mat4 { return m_matrix; }
    fn update_matrix(s32 viewport_x, s32 viewport_y) -> void;

    Mode mode = Mode::None;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    f32  near_plane = 0.f;
    f32  far_plane = 0.f;
    
    // @Note: 2D Specific
    f32 size = 0.f;

    // @Note: 3D Specific
    f32 fov = 0.f;

private:
    Mat4 m_matrix;
};

// @Note: Axis Aligned Bounding Box.
struct AABB {
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 half_w = 0.0f;
    f32 half_h = 0.0f;

    static fn overlap(const AABB& a, const AABB& b) -> bool;
};
