#ifndef VECTOR3_H
#define VECTOR3_H
namespace wlEngine {
    template <typename T>
    struct Vector3 {
        T x;
        T y;
        T z;

        Vector3 operator+(const Vector3& other) {
            return Vector3<T>{x + other.x, y + other.y, z + other.z};
        };
        Vector3 operator-(const Vector3& other){
            return Vector3<T>{x - other.x, y - other.y, z - other.z};
        }
    };
}

#endif
