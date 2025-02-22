#ifndef FPOINT3_H
#define FPOINT3_H

struct fPoint3 {
    float x, y, z;
    fPoint3 operator+(const fPoint3& other);
    fPoint3 operator-(const fPoint3& other);
    fPoint3 operator*(float scalar);
    fPoint3 normalize();
};

#endif