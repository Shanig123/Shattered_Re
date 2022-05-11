#pragma once

#ifndef __FILEINFO_H__
#define __FILEINFO_H__

USING(Engine)

class CFileInfo final
{
public:
	explicit CFileInfo();
	virtual ~CFileInfo() = default;

public:
	static const wstring ConvertRelativePath(const wstring& _FileFullPath);
	static void FilePathExtraction(const wstring& _FileFullPath, vector<wstring>& _rVecFilePath, vector<wstring>& _rVecFileName, vector<wstring>& _rVecFileTitle);
};

#endif // !__FILEINFO_H__