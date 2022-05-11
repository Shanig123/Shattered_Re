#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}

const wstring CFileInfo::ConvertRelativePath(const wstring& _FileFullPath)
{
	TCHAR szRelativePath[MAX_PATH] = L"";
	TCHAR szCurrentPath[MAX_PATH] = L"";

	// ���� ���(���� ���)
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	// ���� ��θ� ��� ��� ����
	PathRelativePathTo(szRelativePath
		, szCurrentPath
		, FILE_ATTRIBUTE_DIRECTORY
		, _FileFullPath.c_str()
		, FILE_ATTRIBUTE_DIRECTORY);

	// ��ο� ���ϻ���
	PathRemoveFileSpec(szRelativePath);
	lstrcat(szRelativePath, L"\\");

	wstring wstrOut = szRelativePath;

	return wstrOut;
}

void CFileInfo::FilePathExtraction(const wstring& _FileFullPath, vector<wstring>& _rVecFilePath, vector<wstring>& _rVecFileName, vector<wstring>& _rVecFileTitle)
{
	TCHAR szFileName[MAX_PATH] = L"";
	TCHAR szFileTitle[MAX_PATH] = L"";

	CFileFind find;
	wstring wstrPath = _FileFullPath + L"\\*.*";
	BOOL bContinue = find.FindFile(wstrPath.c_str());

	while (bContinue)
	{
		bContinue = find.FindNextFileW();

		if (find.IsDots())
			continue;
		if (find.IsDirectory())
		{
			FilePathExtraction(find.GetFilePath().GetString(), _rVecFilePath, _rVecFileName, _rVecFileTitle);
		}
		else
		{
			if (find.IsSystem())
				continue;

			// ���ϸ� ���, Ȯ���� ����
			lstrcpy(szFileName, find.GetFileName());
			// ���ϸ� ���, Ȯ���� ������
			lstrcpy(szFileTitle, find.GetFileTitle());

			//���� ��� ���
			TCHAR szRelativePath[MAX_PATH] = L"";
			TCHAR szCurrentPath[MAX_PATH] = L"";

			// ���� ���(���� ���)
			GetCurrentDirectory(MAX_PATH, szCurrentPath);
			// ���� ��θ� ��� ��� ����
			PathRelativePathTo(szRelativePath
				, szCurrentPath
				, FILE_ATTRIBUTE_DIRECTORY
				, find.GetFilePath().GetString()
				, FILE_ATTRIBUTE_DIRECTORY);

			// ��ο� ���ϻ���
			PathRemoveFileSpec(szRelativePath);
			lstrcat(szRelativePath, L"\\");

			wstring wstrFileName = szFileName;
			wstring wstrFileTitle = szFileTitle;
			wstring wstrFilePath = szRelativePath;

			if (wstrFileName.find(L".X") != wstring::npos)
			{
				_rVecFilePath.push_back(wstrFilePath);
				_rVecFileName.push_back(wstrFileName);
				_rVecFileTitle.push_back(wstrFileTitle);
			}			
		}
	}
}