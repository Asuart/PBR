#pragma once


template <typename T> class Vector2 {
public:
    Vector2() { x = y = 0; }
    Vector2(T xx, T yy)
        : x(xx), y(yy) {
        Assert(!HasNaNs());
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y);
    }
    explicit Vector2(const Point2<T>& p);
    explicit Vector2(const Point3<T>& p);

    friend std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
        os << "[" << v.x << ", " << v.y << "]";
        return os;
    }

    Vector2<T> operator+(const Vector2<T>& v) const {
        Assert(!v.HasNaNs());
        return Vector2(x + v.x, y + v.y);
    }

    Vector2<T>& operator+=(const Vector2<T>& v) {
        Assert(!v.HasNaNs());
        x += v.x; y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Vector2<T>& v) const {
        Assert(!v.HasNaNs());
        return Vector2(x - v.x, y - v.y);
    }

    Vector2<T>& operator-=(const Vector2<T>& v) {
        Assert(!v.HasNaNs());
        x -= v.x; y -= v.y;
        return *this;
    }
    bool operator==(const Vector2<T>& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(const Vector2<T>& v) const {
        return x != v.x || y != v.y;
    }
    Vector2<T> operator*(T f) const { return Vector2<T>(f * x, f * y); }

    Vector2<T>& operator*=(T f) {
        Assert(!std::isnan(f));
        x *= f; y *= f;
        return *this;
    }
    Vector2<T> operator/(T f) const {
        Assert(f != 0);
        float inv = (float)1 / f;
        return Vector2<T>(x * inv, y * inv);
    }

    Vector2<T>& operator/=(T f) {
        Assert(f != 0);
        float inv = (float)1 / f;
        x *= inv; y *= inv;
        return *this;
    }
    Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
    T operator[](int i) const {
        Assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }

    T& operator[](int i) {
        Assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }

    T x, y;

};


template <typename T> class Vector3 {
public:
        T operator[](int i) const {
        Assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    T& operator[](int i) {
        Assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    Vector3() { x = y = z = 0; }
    Vector3(T x, T y, T z)
        : x(x), y(y), z(z) {
        Assert(!HasNaNs());
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }
    explicit Vector3(const Point3<T>& p);
    friend std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
        os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return os;
    }
    Vector3<T> operator+(const Vector3<T>& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3<T>& operator+=(const Vector3<T>& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Vector3<T>& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3<T>& operator-=(const Vector3<T>& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    bool operator==(const Vector3<T>& v) const {
        return x == v.x && y == v.y && z == v.z;
    }
    bool operator!=(const Vector3<T>& v) const {
        return x != v.x || y != v.y || z != v.z;
    }
    Vector3<T> operator*(T s) const { return Vector3<T>(s * x, s * y, s * z); }
    Vector3<T>& operator*=(T s) {
        x *= s; y *= s; z *= s;
        return *this;
    }
    Vector3<T> operator/(T f) const {
        Assert(f != 0);
        float inv = (float)1 / f;
        return Vector3<T>(x * inv, y * inv, z * inv);
    }

    Vector3<T>& operator/=(T f) {
        Assert(f != 0);
        float inv = (float)1 / f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }
    explicit Vector3(const Normal3<T>& n);

    T x, y, z;
};