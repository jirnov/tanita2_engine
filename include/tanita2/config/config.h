/*
 Движок "Танита2".
 Настройки приложения, статические и загружаемые из файла настроек.
 */
#pragma once
#ifndef _TANITA2_CONFIG_CONFIG_H_
#define _TANITA2_CONFIG_CONFIG_H_
#include <tanita2/config/detail/begin_detail.h>

namespace config
{

// Идентификатор ревизии сборки в системе контроля версий.
// Изменяется автоматически при каждой сборке.
CONFIG_STATIC_VALUE(projectBuildRevision, const wchar_t *);

// Ревизия сборки неизвестна или содержит локальные изменения.
CONFIG_STATIC_VALUE(projectIsNotVersioned, bool);

// Название проекта.
CONFIG_STATIC_VALUE(projectName, const wchar_t *);

// Имя лог-файла.
CONFIG_STATIC_VALUE(logFileName, const wchar_t *);

// Настройки потоков вывода профилирования и отладки.
CONFIG_VALUE(profileStreamEnabled, bool, true);
CONFIG_VALUE(debugStreamEnabled, bool, true);

namespace gfx
{

// Поддержка NV PerfHUD.
// TODO: значение по умолчанию - false.
CONFIG_VALUE(perfHUDEnabled, bool, true);

} // namespace gfx

} // namespace config

#include <tanita2/config/detail/end_detail.h>
#endif // _TANITA2_CONFIG_CONFIG_H_
