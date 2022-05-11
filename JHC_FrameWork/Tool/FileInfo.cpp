#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}

const wstring CFileInfo::ConvertRelativePath(const wstring& _FileFullPath)
{
	TCHAR szRelativePath[MAX_PATH] = L"";
	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 경로(절대 경로)
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	// 절대 경로를 상대 경로 변경
	PathRelativePathTo(szRelativePath
		, szCurrentPath
		, FILE_ATTRIBUTE_DIRECTORY
		, _FileFullPath.c_str()
		, FILE_ATTRIBUTE_DIRECTORY);

	// 경로에 파일삭제
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

			// 파일명 출력, 확장자 포함
			lstrcpy(szFileName, find.GetFileName());
			// 파일명 출력, 확장자 미포함
			lstrcpy(szFileTitle, find.GetFileTitle());

			//파일 경로 출력
			TCHAR szRelativePath[MAX_PATH] = L"";
			TCHAR szCurrentPath[MAX_PATH] = L"";

			// 현재 경로(절대 경로)
			GetCurrentDirectory(MAX_PATH, szCurrentPath);
			// 절대 경로를 상대 경로 변경
			PathRelativePathTo(szRelativePath
				, szCurrentPath
				, FILE_ATTRIBUTE_DIRECTORY
				, find.GetFilePath().GetString()
				, FILE_ATTRIBUTE_DIRECTORY);

			// 경로에 파일삭제
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