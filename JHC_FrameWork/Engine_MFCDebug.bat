//명령어		옵션			복사할 경로					복사받을 경로
xcopy			/y				.\Engine\Headers\*.h				.\MFCRef\Headers\
xcopy			/y				.\Engine\Codes\*.h					.\MFCRef\Headers\
xcopy			/y				.\Engine\Headers\*.hpp				.\MFCRef\Headers\

xcopy			/y				.\Engine\Bin_MFCDebug\*.dll			.\Tool\Bin_Debug\

xcopy			/y				.\Engine\Bin_MFCDebug\*.lib			.\MFCRef\Libraries\