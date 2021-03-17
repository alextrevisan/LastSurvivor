#include <psxgte.h>

struct Vector: VECTOR
{
    Vector():VECTOR{0,0,0}{}
    Vector(int x, int y, int z):VECTOR{x,y,z}{};
    Vector& operator*=(int value)
    {
        vx *= value;
        vy *= value;
        vz *= value;
        return *this;
    }
    
    Vector operator*(int value) const
    {
        return {vx*value,vy*value,vz*value};
    }

    Vector operator+=(const Vector& other)
    {
        vx += other.vx;
        vy += other.vy;
        vz += other.vz;
        return *this;
    }
};

struct SVector: SVECTOR
{
    SVector():SVECTOR{0,0,0}{}
    SVector(short x, short y, short z):SVECTOR{x,y,z}{};
    SVector& operator*=(int value)
    {
        vx *= value;
        vy *= value;
        vz *= value;
        return *this;
    }
    
    SVector operator*(int value) const
    {
        return {vx*value,vy*value,vz*value};
    }

    SVector operator+=(const SVector& other)
    {
        vx += other.vx;
        vy += other.vy;
        vz += other.vz;
        return *this;
    }
};

class Camera
{
public:
    Camera();
    void SetPosition(int x, int y, int z);
    void SetPosition(const VECTOR& pos);
    void SetRotation(int x, int y, int z = 0);
    void SetRotation(const VECTOR& rot);
    void Update();
    void LookAt(const VECTOR& pos);
    void Translate(const VECTOR& translation);
    void Translate(const SVECTOR& translation);
    void Translate(int x, int y, int z);
    void Roll(int x);
    void Pich(int y);
    void Yaw(int z);
    const SVector Forward() const;
    const SVector Right() const;
    const SVector Up() const;
    VECTOR& Position();
    const VECTOR& Rotation() const;
    const MATRIX& Matrix() const;
private:
    void LookAt(const VECTOR &eye, const VECTOR& at, const SVECTOR& up);
    VECTOR CrossProduct(const SVECTOR& v0, const SVECTOR& v1) const;
    VECTOR mPosition{0,0,0};
    VECTOR mRotation{0,0,0};
    MATRIX	mMatrix{0};
};