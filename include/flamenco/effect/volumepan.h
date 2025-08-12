/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 
 Эффект для изменения громкости и панорамирования.
 */
#pragma once
#ifndef _FLAMENCO_EFFECT_VOLUMEPAN_H_
#define _FLAMENCO_EFFECT_VOLUMEPAN_H_

namespace flamenco
{

// Эффект для изменения громкости и панорамирования.
class volume_pan : public effect
{
public:
    // Создание нового эффекта. Громкость задается числом от 0 до 1,
    // панорама - от -1 до 1.
    static reference<volume_pan> create( reference<pin> input, f32 volume=1.0f, f32 pan=0.0f );
    
    
    // Громкость в диапазоне от 0 до 1.
    atomic<f32> volume;
    // Баланс (панорама).
    atomic<f32> pan;
    
    
    // Примечание: даже если значения громкости или баланса выйдут из
    // разрешенного диапазона, внутри эффекта будут использоваться
    // приведенные к нему значения.
    
    
private:
    volume_pan( reference<pin> input, f32 volume, f32 pan );
    
    // Заполняет буферы каналов звуковыми данными.
    void process( f32 * left, f32 * right );
    
    // volume и pan сделаны публичной переменной без get/set, т.к. atomic_f32 уже
    // содержит свои get и set, код был бы похож на лапшу.
};

} // namespace flamenco

#endif // _FLAMENCO_EFFECT_VOLUMEPAN_H_
