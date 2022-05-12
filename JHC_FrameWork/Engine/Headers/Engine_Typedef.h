#pragma once

#ifndef __ENGINE_TYPEDEF_H__
#define __ENGINE_TYPEDEF_H__

typedef bool						_bool;

typedef char						_char, _byte;
typedef unsigned char				_uchar, _ubyte;
typedef TCHAR						_tchar;

typedef int							_int;
typedef unsigned int				_uint, _EVENT;
typedef _LARGE_INTEGER				_lint;
typedef signed long					_long;
typedef unsigned long				_ulong;
typedef float						_float;
typedef double						_double;

typedef XMFLOAT2					_float2;
typedef XMFLOAT3					_float3;
typedef XMFLOAT4					_float4;
typedef XMFLOAT4					_float4Q;
typedef XMFLOAT4X4					_float4x4;

//XMVECTOR

//typedef LPDIRECT3D9					_SDK;
//typedef LPDIRECT3DDEVICE9			_DEVICE;
typedef ID3D11Device*			_DEVICE;

//typedef LPD3DXFONT					_FONT;
typedef D3DCOLOR					_COLOR;

//typedef LPDIRECT3DVERTEXBUFFER9		_VertexBuffer;
//typedef LPDIRECT3DINDEXBUFFER9		_IndexBuffer;
//typedef	LPD3DXBUFFER				_BUFFER;

//typedef LPD3DXMESH					_MESH;
//typedef LPD3DXMATERIAL				_MATERIAL;
//typedef LPD3DXANIMATIONCONTROLLER	_ANICONTROLLER;
//typedef LPDIRECT3DTEXTURE9			_TEXTURE9;

typedef ID3D10Effect*				_EFFECT;
//typedef LPDIRECT3DSURFACE9			_SURFACE9;


#endif // !__ENGINE_TYPEDEF_H__
