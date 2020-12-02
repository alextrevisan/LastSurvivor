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
    void SetPosition(const Vector& pos);
    void SetRotation(int x, int y, int z = 0);
    void SetRotation(const Vector& rot);
    void Update();
    void LookAt(const Vector& pos);
    void Translate(const Vector& translation);
    void Translate(const SVector& translation);
    void Translate(int x, int y, int z);
    void Roll(int x);
    void Pich(int y);
    void Yaw(int z);
    SVector Forward() const;
    SVector Right() const;
    SVector Up() const;
    Vector Position() const;
    SVector Rotation() const;
    MATRIX Matrix() const;
private:
    void LookAt(const Vector &eye, const Vector& at, const SVector& up);
    Vector CrossProduct(const SVector& v0, const SVector& v1) const;
    Vector mPosition;
    Vector mRotation;
    MATRIX	mMatrix;
};