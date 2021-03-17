#include "camera.h"
#include <psxgpu.h>

Camera::Camera()
{

}

void Camera::SetPosition(int x, int y, int z)
{
    mPosition = {x, y, z};
}

void Camera::SetPosition(const VECTOR& pos)
{
    SetPosition(pos.vx, pos.vy, pos.vz);
}

void Camera::SetRotation(int x, int y, int z)
{
    mRotation = {ONE*x, ONE*y, ONE*z};
}

void Camera::SetRotation(const VECTOR& rot)
{
    mRotation = rot;
}

void Camera::Update()
{
    Vector transformedPosition{-mPosition.vx>>12, -mPosition.vy>>12, -mPosition.vz>>12};
    SVECTOR trot{mRotation.vx, mRotation.vy, mRotation.vz};
    RotMatrix(&trot, &mMatrix);
    // Apply rotation of matrix to translation value to achieve a
    // first person perspective
    ApplyMatrixLV( &mMatrix, &transformedPosition, &transformedPosition );
			
	// Set translation matrix
    TransMatrix( &mMatrix, &transformedPosition );
}   

void Camera::Translate(int x, int y, int z)
{
    mPosition.vx += x;
    mPosition.vy += y;
    mPosition.vz += z;
}

void Camera::Translate(const VECTOR& translation)
{
    mPosition.vx += translation.vx;
    mPosition.vy += translation.vy;
    mPosition.vz += translation.vz;
}

void Camera::Translate(const SVECTOR& translation)
{
    mPosition.vx += translation.vx;
    mPosition.vy += translation.vy;
    mPosition.vz += translation.vz;
}

void Camera::Roll(int x)
{
    mRotation.vx += x >> 5;
}
void Camera::Pich(int y)
{
    mRotation.vy += y >> 5;
}
void Camera::Yaw(int z)
{
    mRotation.vz += z >> 5;
}

const SVector Camera::Forward() const
{
    const SVector rot{mRotation.vx,mRotation.vy,mRotation.vz};
    return {(( isin( rot.vy )*icos( rot.vx ) )>>12), isin( -rot.vx ), (( icos( rot.vy )*icos( rot.vx ) )>>12)};
}

const SVector Camera::Right() const
{
    const SVector rot{mRotation.vx<<12,mRotation.vy<<12,mRotation.vz<<12};
    return {icos( rot.vy ), 0, isin( rot.vy )};
}

const SVector Camera::Up() const
{
    const SVector rot{mRotation.vx<<12,mRotation.vy<<12,mRotation.vz<<12};
    return {-(( isin( rot.vy )*isin( rot.vx ) )>>12), -icos( rot.vx ), (( icos( rot.vy )*isin( rot.vx ) )>>12)};
}

VECTOR& Camera::Position()
{   
    return mPosition;
}

const VECTOR& Camera::Rotation() const
{   
    return mRotation;
}

const MATRIX& Camera::Matrix() const
{
    return mMatrix;
}

void Camera::LookAt(const VECTOR& pos)
{
    // Vector that defines the 'up' direction of the camera
    SVECTOR up = { 0, -ONE, 0 };
    // Divide out fractions of camera coordinates
    const VECTOR translationValues{mPosition.vx>>12, mPosition.vy>>12, mPosition.vz>>12};
    
    // Look at the cube
    LookAt(translationValues, pos, up);
}

void Camera::LookAt(const VECTOR &eye, const VECTOR& at, const SVECTOR& up)
{
	SVECTOR zaxis;
	SVECTOR xaxis;
	SVECTOR yaxis;

	VECTOR taxis{at.vx - eye.vx, at.vy - eye.vy, at.vz - eye.vz};
	VectorNormalS(&taxis, &zaxis);
    taxis = CrossProduct(zaxis, up);
	VectorNormalS(&taxis, &xaxis);
	taxis = CrossProduct(zaxis, xaxis);
	VectorNormalS(&taxis, &yaxis);

	mMatrix.m[0][0] = xaxis.vx;	mMatrix.m[1][0] = yaxis.vx;	mMatrix.m[2][0] = zaxis.vx;
	mMatrix.m[0][1] = xaxis.vy;	mMatrix.m[1][1] = yaxis.vy;	mMatrix.m[2][1] = zaxis.vy;
	mMatrix.m[0][2] = xaxis.vz;	mMatrix.m[1][2] = yaxis.vz;	mMatrix.m[2][2] = zaxis.vz;

	Vector pos{-eye.vx, -eye.vy, -eye.vz};
    Vector vec{0,0,0};
	ApplyMatrixLV(&mMatrix, &pos, &vec);
	TransMatrix(&mMatrix, &vec);
}

VECTOR Camera::CrossProduct(const SVECTOR& v0, const SVECTOR& v1) const 
{
    return VECTOR{
        ((v0.vy*v1.vz)-(v0.vz*v1.vy))>>12,
	    ((v0.vz*v1.vx)-(v0.vx*v1.vz))>>12,
	    ((v0.vx*v1.vy)-(v0.vy*v1.vx))>>12
    };
}