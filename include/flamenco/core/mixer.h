/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Микшер.
 */
#pragma once
#ifndef _FLAMENCO_CORE_MIXER_H_
#define _FLAMENCO_CORE_MIXER_H_

#include <vector>

namespace flamenco
{

// Микшер.
class mixer : noncopyable
{
public:
    // Получение ссылки на единственную копию микшера.
    static mixer & singleton();
    
    
    // Присоединение пина. assert(!pin.connected()).
    void attach( reference<pin> pin );
    // Отсоединение пина. Если он не присоединен, warning в лог.
    void detach( reference<pin> pin );
    
    // Заполняет буфер смикшированными данными. Формат буфера - PCM 44100 16bit interleaved stereo.
    // Размер буфера фиксирован - равен MIXER_BUFFER_SIZE_IN_SAMPLES семплам.
    //
    // Функция в цикле пробегает по подключенным пинам и вызывает их функцию process() от своих
    // внутренних буферов. Затем смешивает получившиеся данные в итоговый буфер.
    //
    // Пара слов о реализации: внутри микшера должно быть 2 буфера - по одному на канал для передачи
    // в pin::process. В таком случае после получения данных пина они должны быть примешаны к итоговому
    // буферу. (Возможно, это будет медленно, тогда нужно будет еще 2 буфера для смешивания отдельно каналов,
    // а затем уже их копировать с чередованием в переданный буфер).
    // Для проверки выхода за границы массива хорошо делать внутренние буферы чуть больше, чем нужно
    // и забивать их известными значениями.
    //
    // Примечание: пин, присоединенный непосредственно к микшеру, при вызове process получит буферы
    // каналов, заполненные тишиной.
    void mix( s16 * buffer );
    
private:
    // Критическая секция.
    static critical_section mCriticalSection;
    // Список подключенных пинов.
    typedef std::vector<reference<pin> > PinList;
    PinList mPins;

    template <class T>
    struct aligned_deleter
    {
        void operator()(f32 * ptr)
        {
            _aligned_free(ptr);
        }
    };
    
    // Буферы для левого и правого каналов.
    // Должны быть выровнены по границе 16.
    auto_array<f32, aligned_deleter> mBufferL, mBufferR;
    
    // Буфер для внутренних нужд.
    auto_array<f32, aligned_deleter> mMixerBuffer;
    
    // Волшебное значение для проверки выхода за границы буферов.
    static const f32 MAGIC;
    
    mixer();
    ~mixer();
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_MIXER_H_
