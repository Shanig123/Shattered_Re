#pragma once

#ifndef __CLIENT_DEFINE_H__
#define __CLIENT_DEFINE_H__


#ifdef _DEBUG

#define WINCX 1280
#define WINCY 720
#else

//#define WINCX 1920
//#define WINCY 1080
#define WINCX 1280
#define WINCY 720
#endif // _DEBUG


#define OBJECT_SIZE {0.015f, 0.015f, 0.015f}
#define BGM_VOLUME 0.3f
#define STAGE2_CULL 60.f

#endif // !__CLIENT_DEFINE_H__
