//명령어		옵션			복사할 경로					복사받을 경로
xcopy			/y				.\Engine\Headers\*.h		.\Reference\Headers\
xcopy			/y				.\Engine\Codes\*.h			.\Reference\Headers\
xcopy			/y				.\Engine\Headers\*.hpp		.\Reference\Headers\

xcopy			/y				.\Engine\PhysX\*.h								.\Reference\Headers\PhysX\
xcopy			/y				.\Engine\PhysX\characterkinematic\*.h			.\Reference\Headers\PhysX\characterkinematic\
xcopy			/y				.\Engine\PhysX\characterkinematic\src\*.h		.\Reference\Headers\PhysX\characterkinematic\src\
xcopy			/y				.\Engine\PhysX\cloth\*.h						.\Reference\Headers\PhysX\cloth\
xcopy			/y				.\Engine\PhysX\collision\*.h					.\Reference\Headers\PhysX\collision\
xcopy			/y				.\Engine\PhysX\common\*.h						.\Reference\Headers\PhysX\common\
xcopy			/y				.\Engine\PhysX\cooking\*.h						.\Reference\Headers\PhysX\cooking\
xcopy			/y				.\Engine\PhysX\cudamanager\*.h					.\Reference\Headers\PhysX\cudamanager\
xcopy			/y				.\Engine\PhysX\extensions\*.h					.\Reference\Headers\PhysX\extensions\
xcopy			/y				.\Engine\PhysX\filebuf\*.h						.\Reference\Headers\PhysX\filebuf\
xcopy			/y				.\Engine\PhysX\foundation\*.h					.\Reference\Headers\PhysX\foundation\
xcopy			/y				.\Engine\PhysX\foundation\unix\*.h				.\Reference\Headers\PhysX\foundation\unix\
xcopy			/y				.\Engine\PhysX\foundation\windows\*.h			.\Reference\Headers\PhysX\foundation\windows\
xcopy			/y				.\Engine\PhysX\geometry\*.h						.\Reference\Headers\PhysX\geometry\
xcopy			/y				.\Engine\PhysX\GeomUtils\*.h					.\Reference\Headers\PhysX\GeomUtils\
xcopy			/y				.\Engine\PhysX\gpu\*.h							.\Reference\Headers\PhysX\gpu\
xcopy			/y				.\Engine\PhysX\particles\*.h					.\Reference\Headers\PhysX\particles\
xcopy			/y				.\Engine\PhysX\pvd\*.h							.\Reference\Headers\PhysX\pvd\
xcopy			/y				.\Engine\PhysX\solver\*.h						.\Reference\Headers\PhysX\solver\
xcopy			/y				.\Engine\PhysX\task\*.h							.\Reference\Headers\PhysX\task\
xcopy			/y				.\Engine\PhysX\vehicle\*.h						.\Reference\Headers\PhysX\vehicle\


xcopy			/y				.\Engine\Bin_Debug\*.dll			.\Client\Bin_Debug\
//xcopy			/y				.\Engine\Bin_Debug\*.dll			.\Tool\Bin_Debug\

xcopy			/y				.\Engine\Bin_Debug\*.lib			.\Reference\Libraries\