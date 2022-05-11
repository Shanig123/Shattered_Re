#pragma once

#ifndef __ENGINE_FUNCTIONS_H__
#define __ENGINE_FUNCTIONS_H__

template <typename T>
_uint Safe_AddRef(T& _ptr)
{
	_uint iRefCount = 0;

	if (NULL != _ptr)
		iRefCount = _ptr->AddRef();

	return iRefCount;
}

template <typename T>
_uint Safe_Release(T& _ptr)
{
	_uint	iRefCount = 0;

	if (NULL != _ptr)
	{
		iRefCount = _ptr->Release();
		if (iRefCount == 0)
			_ptr = NULL;
	}

	return iRefCount;
}

template <typename T>
void Safe_Delete(T& ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

template <typename T>
void Safe_Delete_Array(T& _ptr)
{
	if (_ptr)
	{
		delete[] _ptr;
		_ptr = nullptr;
	}
}

// 연관컨테이너 삭제용
class CDeleteMap
{
public:
	explicit CDeleteMap(void) {}
	~CDeleteMap(void) {}
public: // operator	
	template <typename T>
	void operator () (T& Pair)
	{
		_ulong dwRefCnt = 0;

		dwRefCnt = Pair.second->Release();

		if (0 == dwRefCnt)
			Pair.second = NULL;
	}
};

class CDeleteObj
{
public:
	explicit CDeleteObj(void) {}
	~CDeleteObj(void) {}
public: // operator
	template <typename T>
	void operator () (T& Pair)
	{
		_ulong dwRefCnt = 0;

		dwRefCnt = Pair->Release();

		if (0 == dwRefCnt)
			Pair = nullptr;
	}
};

static const wstring CharToWChar(const char* _pStr)
{
	_uint iLength = (_uint)strlen(_pStr) + 1;
	_uint iSize_needed = MultiByteToWideChar(CP_UTF8, 0, _pStr, iLength, NULL, 0);
	wstring wstr(iSize_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, _pStr, iLength, &wstr[0], iSize_needed);

	return wstr;
}

static const char* WstringToChar(const wstring& _pStr)
{
	size_t cn;
	_uint iLength = (_uint)wcslen(_pStr.c_str()) + 1;
	char* pChar = (char*)malloc(sizeof(char) * iLength);

	wcstombs_s(&cn, pChar, iLength, _pStr.c_str(), iLength);

	return pChar;
}

#ifdef _AFX
static const char* CStringToChar(const CString& _pStr)
{
	size_t cn;
	_uint iLength = (_uint)wcslen(_pStr.GetString()) + 1;
	char* pChar = (char*)malloc(sizeof(char) * iLength);

	wcstombs_s(&cn, pChar, iLength, _pStr.GetString(), iLength);

	return pChar;
}
#endif // !_AFX

// 문자열 자르기
static void SplitW(const TCHAR _szPattern, const wstring& _wstr, vector<wstring>* _vecSplit)
{
	wstringstream wstrstream(_wstr);
	wstring tocken;
	vector<wstring> vecTemp;

	while (getline(wstrstream, tocken, _szPattern))
	{
		vecTemp.push_back(tocken);
	}

	*_vecSplit = vecTemp;
}

// 파일 존재 여부 확인
static _bool FileFind(const wstring& _wstrFilePath)
{
	const char* FilePath = WstringToChar(_wstrFilePath);

	FILE* File;
	fopen_s(&File, FilePath, "r");

	if (File == NULL)			// 파일 존재 하지 않음
	{
		Safe_Delete(FilePath);
		return false;
	}
	else						// 파일 존재함
	{
		fclose(File);
		Safe_Delete(FilePath);
		return true;
	}
}

inline static void Print_Debug_Bool(const wstring& _pMessagetag, const _bool& _rbValue)
{
	wstring wstrPrint = _pMessagetag + L" : " + (_rbValue ? L"true" : L"false") + L"\n";
	OutputDebugStringW(wstrPrint.c_str());
}

inline static void Print_Debug_Float(const wstring& _pMessagetag, const _float& _rfValue)
{
	wstring wstrPrint = _pMessagetag + L" : " + to_wstring(_rfValue) + L"\n";
	OutputDebugStringW(wstrPrint.c_str());
}

inline static void Print_Debug_Int(const wstring& _pMessagetag, const _int& _riValue)
{
	wstring wstrPrint = _pMessagetag + L" : " + to_wstring(_riValue) + L"\n";
	OutputDebugStringW(wstrPrint.c_str());
}

inline static void Print_Debug_Str(const wstring& _pMessagetag, const wstring& _pszValue)
{
	wstring wstrPrint = _pMessagetag + L" : " + _pszValue + L"\n";
	OutputDebugStringW(wstrPrint.c_str());
}

inline static void Print_Debug_Str(const wstring& _pszValue)
{
	wstring wstrPrint = _pszValue + L"\n";
	OutputDebugStringW(wstrPrint.c_str());
}


static auto Mersen_ToFloat(_float _fMin, _float _fMax)
{
	random_device random;
	mt19937 engine(random());								// MT19937 난수 엔진(seed값 설정)	
	uniform_real_distribution<_float> distribution(_fMin, _fMax);	// 난수 생성 범위
	auto generated = distribution(engine);
	return generated;								// 난수 생성 함수
}
static auto Mersen_ToInt(_int _iMin, _int _iMax)
{
	random_device random;
	mt19937 engine(random());								// MT19937 난수 엔진(seed값 설정)	
	uniform_int_distribution<_int> distribution(_iMin, _iMax);	// 난수 생성 범위
	auto generated = distribution(engine);
	return generated;								// 난수 생성 함수
}

inline static _bool CoolTime(const _float& _fDeltaTime, _float& _AddTime, const _float& _fStopTime)
{
	_AddTime += _fDeltaTime;
	return _AddTime > _fStopTime ? (true) : (false);
}
/*
Note:	D3DCOLOR_ARGB와 같습니다.
*/
inline static D3DCOLOR Argb(_ARGB _tArgb) { return D3DCOLOR_ARGB(_tArgb.ubyA, _tArgb.ubyR, _tArgb.ubyG, _tArgb.ubyB); }
/*
Note:	D3DCOLOR_ARGB와 같습니다.
*/
inline static D3DCOLOR	Argb(_ubyte _ubyA = 255, _ubyte _ubyR = 255, _ubyte _ubyG = 255, _ubyte _ubyB = 255) { return D3DCOLOR_ARGB(_ubyA, _ubyR, _ubyG, _ubyB); }

// 포물선 운동 공식
inline static _float Jump_Function_To_Y(_float* _fYIn, _float _fDeltaTime, _float _fPower)
{
	if (!_fYIn)
		return 0.f;

	_float fYTemp = *_fYIn;
	return fYTemp = fYTemp + (_fPower*_fDeltaTime - 0.5f * _float(GRAVITY) *_fDeltaTime * _fDeltaTime);
}

/* a to b , t = time or speed*/
inline static _float4Q Quaternion_LERP(const _float4Q& a, const _float4Q& b, const _float t)
{
	_float4Q r;
	_float t_ = 1 - t;
	r.x = t_*a.x + t*b.x;
	r.y = t_*a.y + t*b.y;
	r.z = t_*a.z + t*b.z;
	r.w = t_*a.w + t*b.w;
	D3DXQuaternionNormalize(&r, &r);
	return r;
}
/* a to b , t = time or speed*/
inline static _float4Q Quaternion_SLERP(const _float4Q& a, const _float4Q& b, const _float t)
{
	_float4Q r;
	_float t_ = 1 - t;
	_float Wa, Wb;
	_float theta = acos(a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
	_float sn = sin(theta);
	Wa = sin(t_*theta) / sn;
	Wb = sin(t*theta) / sn;
	r.x = Wa*a.x + Wb*b.x;
	r.y = Wa*a.y + Wb*b.y;
	r.z = Wa*a.z + Wb*b.z;
	r.w = Wa*a.w + Wb*b.w;
	D3DXQuaternionNormalize(&r, &r);
	return r;
}

/* a to b , t = time or speed*/
inline static _float4Q Quaternion_LERP_NoNomal(const _float4Q& a, const _float4Q& b, const _float t)
{
	_float4Q r;
	_float t_ = 1 - t;
	r.x = t_*a.x + t*b.x;
	r.y = t_*a.y + t*b.y;
	r.z = t_*a.z + t*b.z;
	r.w = t_*a.w + t*b.w;
	return r;
}
/* a to b , t = time or speed*/
inline static _float4Q Quaternion_SLERP_NoNomal(const _float4Q& a, const _float4Q& b, const _float t)
{
	_float4Q r;
	_float t_ = 1 - t;
	_float Wa, Wb;
	_float theta = acos(a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
	_float sn = sin(theta);
	Wa = sin(t_*theta) / sn;
	Wb = sin(t*theta) / sn;
	r.x = Wa*a.x + Wb*b.x;
	r.y = Wa*a.y + Wb*b.y;
	r.z = Wa*a.z + Wb*b.z;
	r.w = Wa*a.w + Wb*b.w;
	return r;
}


//최대 공약수를 반환합니다. 유클리드 호제법을 사용합니다.
inline static _uint GCD(const _uint& _iValueA, const _uint& _iValueB)
{
	_uint iAValue = _iValueA, iBValue = _iValueB;
	_uint iCValue;

	while (iBValue != 0)
	{
		iCValue = iAValue % iBValue;
		iAValue = iBValue;
		iBValue = iCValue;
	}

	return iAValue;
}
#endif // !__ENGINE_FUNCTIONS_H__
