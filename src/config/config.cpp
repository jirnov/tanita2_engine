/*
 Движок "Танита2".
 Статические константы настроек приложения.
 */
#include <precompiled.h>
#include <tanita2/config/config.h>

namespace config
{

// Инициализация статической настройки. Лучше всего делать в cpp-файлах,
// чтобы не было перекомпиляции всего проекта.
#define CONFIG_STATIC_VALUE_DEFINITION(name, value_type, value)    \
    value_type name##_type::smValue = value;


// Значения статических настроек.
CONFIG_STATIC_VALUE_DEFINITION(projectName, const wchar_t *, L"Tanita2 Engine Refactored");
CONFIG_STATIC_VALUE_DEFINITION(logFileName, const wchar_t *, L"journal");


#undef CONFIG_STATIC_VALUE_DEFINITION

} // namespace config
