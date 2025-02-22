#include "fPoint3.hpp"

fPoint3 fPoint3::operator+(const fPoint3& other) {
    return {x + other.x, y + other.y, z + other.z};
}

fPoint3 fPoint3::operator-(const fPoint3& other) {
    return {x - other.x, y - other.y, z - other.z};
}

fPoint3 fPoint3::operator*(float scalar) {
    return {x * scalar, y * scalar, z * scalar};
}

fPoint3 fPoint3::normalize() {
    float magnitudeSquared = x * x + y * y + z * z;

    const float threehalfs = 1.5F; 

    float x2 = magnitudeSquared * 0.5F; 
    float n = magnitudeSquared; 
    
    // evil floating point bit level hacking 
    long i = * ( long * ) &n; 
    
    // value is pre-assumed 
    i = 0x5f3759df - ( i >> 1 ); 
    n = * ( float * ) &i; 
    
    n *= ( threehalfs - ( x2 * n * n ) );
    n *= ( threehalfs - ( x2 * n * n ) );

    return {x * n, y * n, z * n};
}