#pragma once

#ifndef __DATALOAD_MANAGER_H__
#define __DATALOAD_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CDataLoad_Manager final : public CBase
{
	DECLARE_SINGLETON(CDataLoad_Manager)

private:
	explicit CDataLoad_Manager();
	virtual ~CDataLoad_Manager() = default;

public:
	HRESULT MapResourcesLoad(const wstring & _wstrPath);
	HRESULT MonsterResourcesLoad(const wstring & _wstrPath);
	HRESULT MonsterLoad(const wstring & _wstrPath);
	HRESULT MapObjectResourcesLoad(const wstring & _wstrPath);
	HRESULT MapObjectLoad(const wstring & _wstrPath);

public:
	vector<RESOURCEINFO_DESC*>& Get_MapResource() { return m_vecMapResource; }
	vector<RESOURCEINFO_DESC*>& Get_MonsterResource() { return m_vecMonsterResource; }
	vector<OBJECTINFO_DESC*>& Get_MonsterInfo() { return m_vecMonsterInfo; }
	vector<RESOURCEINFO_DESC*>& Get_MapObjectResource() { return m_vecMapObjectResource; }
	vector<OBJECTINFO_DESC*>& Get_MapObjectInfo() { return m_vecMapObjectInfo; }
public:
	virtual void Free() override;

private:
	vector<RESOURCEINFO_DESC*>				m_vecMapResource;
	vector<RESOURCEINFO_DESC*>				m_vecMonsterResource;
	vector<OBJECTINFO_DESC*>				m_vecMonsterInfo;
	vector<RESOURCEINFO_DESC*>				m_vecMapObjectResource;
	vector<OBJECTINFO_DESC*>				m_vecMapObjectInfo;
};

END

#endif // !__DATALOAD_MANAGER_H__