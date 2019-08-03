#include "Vector.h"

D3DXMATRIXA16 Matrix::getPerspective(float fovx, float aspect, float znear, float zfar){

	float e = tanf(D3DX_PI * fovx / 360);
	float xScale = (1 / e) / aspect;
	float yScale = 1 / e;

	return D3DXMATRIXA16(xScale, 0.0f, 0.0f, 0.0f,
						 0.0f, yScale, 0.0f, 0.0f,
						 0.0f, 0.0f, zfar / (zfar - znear), 1.0f,
						 0.0f, 0.0f, (zfar * znear) / (znear - zfar), 0.0f);
}

D3DXMATRIXA16 Matrix::getInvPerspective(float fovx, float aspect, float znear, float zfar){

	float e = tanf(D3DX_PI * fovx / 360);


	return D3DXMATRIXA16(e * aspect, 0.0f, 0.0f, 0.0f,
						 0.0f	   , e	 , 0.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, (znear - zfar) / (zfar * znear),
						 0.0f, 0.0f, 1.0 , 1.0 / znear);

}

//http://www.mvps.org/directx/articles/linear_z/linearz.htm
D3DXMATRIXA16 Matrix::getLinearPerspective(float fovx, float aspect, float znear, float zfar){

	float e = tanf(D3DX_PI * fovx / 360);
	float xScale = (1 / e) / aspect;
	float yScale = 1 / e;

	return D3DXMATRIXA16(xScale, 0.0f  , 0.0f, 0.0f,
						 0.0f  , yScale, 0.0f, 0.0f,
						 0.0f  , 0.0f  , 1.0f / (zfar - znear), 1.0f,
						 0.0f  , 0.0f  , znear / (znear - zfar), 0.0f);
	
}



D3DXMATRIXA16 Matrix::getOrthographic(float left, float right, float bottom, float top, float znear, float zfar){

	return D3DXMATRIXA16(2 / (right - left), 0.0f, 0.0f, (right + left) / (left - right),
						 0.0f, 2 / (top - bottom), 0.0f, (top + bottom) / (bottom - top),
						 0.0f, 0.0f, 2 / (znear - zfar), (zfar + znear) / (znear - zfar),
						 0.0f, 0.0f, 0.0f			   , 1.0f);
}

D3DXMATRIXA16 Matrix::getLookAT(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &target, const D3DXVECTOR3 &up){

	D3DXVECTOR3 zAxis = target - eye;
	D3DXVec3Normalize(&zAxis, &zAxis);

	D3DXVECTOR3 xAxis;
	D3DXVec3Cross(&xAxis, &up, &zAxis);
	D3DXVec3Normalize(&xAxis, &xAxis);

	D3DXVECTOR3 yAxis;
	D3DXVec3Cross(&yAxis, &zAxis, &xAxis);
	D3DXVec3Normalize(&yAxis, &yAxis);

	return D3DXMATRIXA16(xAxis[0], yAxis[0], zAxis[0], 0.0,
						 xAxis[1], yAxis[1], zAxis[1], 0.0,
						 xAxis[2], yAxis[2], zAxis[2], 0.0,
						 -D3DXVec3Dot(&xAxis, &eye), -D3DXVec3Dot(&yAxis, &eye), -D3DXVec3Dot(&zAxis, &eye), 1.0);
}

D3DXMATRIXA16 Matrix::getInvLookAT(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &target, const D3DXVECTOR3 &up){

	D3DXVECTOR3 zAxis = target - eye;
	D3DXVec3Normalize(&zAxis, &zAxis);

	D3DXVECTOR3 xAxis;
	D3DXVec3Cross(&xAxis, &up, &zAxis);
	D3DXVec3Normalize(&xAxis, &xAxis);

	D3DXVECTOR3 yAxis;
	D3DXVec3Cross(&yAxis, &zAxis, &xAxis);
	D3DXVec3Normalize(&yAxis, &yAxis);

	return D3DXMATRIXA16(xAxis[0], xAxis[1], xAxis[2], 0.0f,
						 yAxis[0], yAxis[1], yAxis[2], 0.0f,
					     zAxis[0], zAxis[1], zAxis[2], 0.0f,
						 eye[0], eye[1], eye[2], 1.0);
}
