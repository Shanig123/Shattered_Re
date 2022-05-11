#include "DataLoad_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CDataLoad_Manager)

CDataLoad_Manager::CDataLoad_Manager()
{
}

HRESULT CDataLoad_Manager::MapResourcesLoad(const wstring & _wstrPath)
{
	HANDLE hFile = CreateFile(_wstrPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"File Load Failed");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwFilePathByte = 0;
	_float3 vPos;
	_float3 vScale;
	_float3 vRotate;
	DWORD dwColliderCount = 0;
	RESOURCEINFO_DESC* tResourceInfoDesc = nullptr;

	while (true)
	{
		tResourceInfoDesc = new RESOURCEINFO_DESC;

		// FilePath 불러오기
		TCHAR szFileReal[MAX_PATH] = L"..\\";
		TCHAR szFilePath[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFilePath, dwFilePathByte, &dwByte, nullptr);
		lstrcatW(szFileReal, szFilePath);

		// FilePath 불러오기
		TCHAR szFileName[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileName, dwFilePathByte, &dwByte, nullptr);

		// FilePath 불러오기
		TCHAR szFileTitle[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tResourceInfoDesc);
			break;
		}

		wsprintf(tResourceInfoDesc->szFilePath, szFileReal);
		wsprintf(tResourceInfoDesc->szFileName, szFileName);
		wsprintf(tResourceInfoDesc->szFileTitle, szFileTitle);

		if (0 == m_vecMapResource.size())
		{
			m_vecMapResource.push_back(tResourceInfoDesc);
		}

		for (auto& iter : m_vecMapResource)
		{
			wstring wstrFilePullPath = szFileReal;
			wstrFilePullPath += szFileName;
			if (!iter->Find(wstrFilePullPath))
			{
				m_vecMapResource.push_back(tResourceInfoDesc);
				break;
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataLoad_Manager::MonsterResourcesLoad(const wstring & _wstrPath)
{
	HANDLE hFile = CreateFile(_wstrPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"File Load Failed");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwFilePathByte = 0;
	_float3 vPos;
	_float3 vScale;
	_float3 vRotate;
	DWORD dwColliderCount = 0;
	RESOURCEINFO_DESC* tResourceInfoDesc = nullptr;

	while (true)
	{
		tResourceInfoDesc = new RESOURCEINFO_DESC;

		// FilePath 불러오기
		TCHAR szFileReal[MAX_PATH] = L"..\\";
		TCHAR szFilePath[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFilePath, dwFilePathByte, &dwByte, nullptr);
		lstrcatW(szFileReal, szFilePath);

		// FilePath 불러오기
		TCHAR szFileName[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileName, dwFilePathByte, &dwByte, nullptr);

		// FilePath 불러오기
		TCHAR szFileTitle[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tResourceInfoDesc);
			break;
		}
		
		wsprintf(tResourceInfoDesc->szFilePath, szFileReal);
		wsprintf(tResourceInfoDesc->szFileName, szFileName);
		wsprintf(tResourceInfoDesc->szFileTitle, szFileTitle);

		if (0 == m_vecMonsterResource.size())
		{
			m_vecMonsterResource.push_back(tResourceInfoDesc);
		}

		for (auto& iter : m_vecMonsterResource)
		{
			wstring wstrFilePullPath = szFileReal;
			wstrFilePullPath += szFileName;
			if (!iter->Find(wstrFilePullPath))
			{
				m_vecMonsterResource.push_back(tResourceInfoDesc);
				break;
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataLoad_Manager::MonsterLoad(const wstring & _wstrPath)
{
	for (auto& rObj : m_vecMonsterInfo)
	{
		Safe_Delete(rObj);
	}
	m_vecMonsterInfo.clear();

	HANDLE hFile = CreateFile(_wstrPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"File Load Failed");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwFilePathByte = 0;
	_float3 vPos;
	_float3 vScale;
	_float3 vRotate;
	EMeshType eMeshType;
	DWORD dwColliderCount = 0;
	OBJECTINFO_DESC* tObjectInfoDesc = nullptr;

	while (true)
	{
		tObjectInfoDesc = new OBJECTINFO_DESC;

		// 정보 불러오기
		ReadFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &eMeshType, sizeof(DWORD), &dwByte, nullptr);

		// FilePath 불러오기
		TCHAR szFileTitle[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tObjectInfoDesc);
			break;
		}
		
		tObjectInfoDesc->vPos = vPos;
		tObjectInfoDesc->vScale = vScale;
		tObjectInfoDesc->vRotate = vRotate;
		tObjectInfoDesc->eFileType = eMeshType;
		wsprintf(tObjectInfoDesc->szFileTitle, szFileTitle);

		m_vecMonsterInfo.push_back(tObjectInfoDesc);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataLoad_Manager::MapObjectResourcesLoad(const wstring & _wstrPath)
{
	HANDLE hFile = CreateFile(_wstrPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"File Load Failed");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwFilePathByte = 0;
	_float3 vPos;
	_float3 vScale;
	_float3 vRotate;
	DWORD dwColliderCount = 0;
	RESOURCEINFO_DESC* tResourceInfoDesc = nullptr;

	while (true)
	{
		tResourceInfoDesc = new RESOURCEINFO_DESC;

		// FilePath 불러오기
		TCHAR szFileReal[MAX_PATH] = L"..\\";
		TCHAR szFilePath[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFilePath, dwFilePathByte, &dwByte, nullptr);
		lstrcatW(szFileReal, szFilePath);

		// FilePath 불러오기
		TCHAR szFileName[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileName, dwFilePathByte, &dwByte, nullptr);

		// FilePath 불러오기
		TCHAR szFileTitle[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tResourceInfoDesc);
			break;
		}

		wsprintf(tResourceInfoDesc->szFilePath, szFileReal);
		wsprintf(tResourceInfoDesc->szFileName, szFileName);
		wsprintf(tResourceInfoDesc->szFileTitle, szFileTitle);

		if (0 == m_vecMapObjectResource.size())
		{
			m_vecMapObjectResource.push_back(tResourceInfoDesc);
		}

		for (auto& iter : m_vecMapObjectResource)
		{
			wstring wstrFilePullPath = szFileReal;
			wstrFilePullPath += szFileName;
			if (!iter->Find(wstrFilePullPath))
			{
				m_vecMapObjectResource.push_back(tResourceInfoDesc);
				break;
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataLoad_Manager::MapObjectLoad(const wstring & _wstrPath)
{
	for (auto& rObj : m_vecMapObjectInfo)
	{
		Safe_Delete(rObj);
	}
	m_vecMapObjectInfo.clear();

	HANDLE hFile = CreateFile(_wstrPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"File Load Failed");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwFilePathByte = 0;
	_float3 vPos;
	_float3 vScale;
	_float3 vRotate;
	EMeshType eMeshType;
	DWORD dwColliderCount = 0;
	OBJECTINFO_DESC* tObjectInfoDesc = nullptr;

	while (true)
	{
		tObjectInfoDesc = new OBJECTINFO_DESC;

		// 정보 불러오기
		ReadFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &eMeshType, sizeof(DWORD), &dwByte, nullptr);

		// FilePath 불러오기
		TCHAR szFileTitle[MAX_PATH] = L"";
		ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tObjectInfoDesc);
			break;
		}

		tObjectInfoDesc->vPos = vPos;
		tObjectInfoDesc->vScale = vScale;
		tObjectInfoDesc->vRotate = vRotate;
		tObjectInfoDesc->eFileType = eMeshType;
		wsprintf(tObjectInfoDesc->szFileTitle, szFileTitle);

		m_vecMapObjectInfo.push_back(tObjectInfoDesc);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CDataLoad_Manager::Free()
{
	for (auto& rObj : m_vecMapResource)
	{
		Safe_Delete(rObj);
	}
	m_vecMapResource.clear();

	for (auto& rObj : m_vecMonsterResource)
	{
		Safe_Delete(rObj);
	}
	m_vecMonsterResource.clear();

	for (auto& rObj : m_vecMonsterInfo)
	{
		Safe_Delete(rObj);
	}
	m_vecMonsterInfo.clear();

	for (auto& rObj : m_vecMapObjectResource)
	{
		Safe_Delete(rObj);
	}
	m_vecMapObjectResource.clear();	

	for (auto& rObj : m_vecMapObjectInfo)
	{
		Safe_Delete(rObj);
	}
	m_vecMapObjectInfo.clear();
}
