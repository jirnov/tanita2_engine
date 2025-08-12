/*
 Движок "Танита2".
 Предкомпилированный заголовочный файл. Должен включаться первым по очереди
 в каждый .cpp файл для ускорения сборки.
 */
#pragma once
#ifndef _TANITA2_PRECOMPILED_H_
#define _TANITA2_PRECOMPILED_H_

// Возможна только юникодная сборка.
// (оба макроса используются в заголовках Windows)
#define UNICODE
#define _UNICODE


// Отладка и assertion.
#include <cassert>

// STL.
#include <string>
#include <vector>

#pragma warning(push,1)
// Boost.
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/python/call.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/const_string/const_string.hpp>
#pragma warning(pop)

// Python.
#include <python/Python.h>

// Заголовки Windows.
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // Windows XP
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>

// DirectX.
#define D3D_DEBUG_INFO
#include <dxerr9.h>
#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#include <dinput.h>
#undef DIRECTINPUT_VERSION

// std::iostream
#include <iostream>
#include <iomanip>

// std::map
#include <map>

// Ogg headers
#include <ogg/ogg.h>
#include <theora/theoradec.h>
#include <vorbis/codec.h>



// Предупреждения компилятора.
#include <tanita2/utility/warnings.h>
// Типы, используемые движком.
#include <tanita2/utility/types.h>
// Стандартные исключения движка.
#include <tanita2/exception/exception.h>
// Система журналирования.
#include <tanita2/logger/logger.h>
// Звуковая система
#include <flamenco/flamenco.h>

// Короткая ссылка к boost::python
namespace bp = boost::python;

// Выключаем все предупреждения
#pragma warning(push, 1)

// Deprecated
#include "base/types.h"
#include "config/config.h"
#include "Tanita2_.h"
#include "Helpers_.h"

#endif // _TANITA2_PRECOMPILED_H_
