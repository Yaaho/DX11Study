#pragma once
// Stdafx.h : 자주 사용하지만 자주 변경되지 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어있는 포함 파일입니다.

#define _WIN32_WINNT 0x0600

#include "TargetVer.h"

// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다
#define WIN32_LEAN_AND_MEAN 
// Windows 헤더 파일
#include <Windows.h>

// C 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 프로그램에 필요한 추가 헤더
#include "DxDefine.h"