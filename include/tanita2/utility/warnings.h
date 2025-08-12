/*
 Движок "Танита2".
 Включение полезных и отключение бесполезных предупреждений компилятора.
 */
#pragma once
#ifndef _TANITA2_UTILITY_WARNINGS_H_
#define _TANITA2_UTILITY_WARNINGS_H_

// Максимальный уровень.
#pragma warning(push,4)

/*
 * Включение предупреждений, выключенных по умолчанию.
 */

// Одно из значений enum не проверяется в switch.
#pragma warning(default:4061)
// Небезопасное преобразование типов.
#pragma warning(default:4905 4906 4946)
// Невиртуальная функция с тем же именем, что и виртуальная.
#pragma warning(default:4263)
// Сокрытие виртуальной функции.
#pragma warning(default:4264 4266)
// Класс с виртуальными ф-циями без виртуального деструктора.
#pragma warning(default:4265)
// Знаковое сравнение с беззнаковым.
#pragma warning(default:4287 4296 4365)
// Функция незаинлайнилась.
#pragma warning(default:4266)
// Пустая операция.
#pragma warning(default:4555)
// Вместо обычной функции вызвана функция-шаблон.
#pragma warning(default:4347)
// Локальный или безымянный тип как аргумент шаблона.
#pragma warning(default:4836)

#endif // _TANITA2_UTILITY_WARNINGS_H_
