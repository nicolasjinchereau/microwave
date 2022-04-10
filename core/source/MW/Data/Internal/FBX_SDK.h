/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#define FBXSDK_SHARED 1
#define FBXSDK_DEFINE_NAMESPACE 1
#define FBXSDK_NAMESPACE_USING 0

#ifndef WIN32
    #define WIN32 1
#endif

#ifndef _MBCS
    #define _MBCS 1
#endif

#include <fbxsdk.h>
