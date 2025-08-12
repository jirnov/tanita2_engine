/*
 Движок "Танита2".
 Менеджер вершинных буферов.
 */
#pragma once
#ifndef _TANITA2_RENDER_VB_VBTYPES_H_
#define _TANITA2_RENDER_VB_VBTYPES_H_

#include <boost/noncopyable.hpp>
#include <tanita2/utility/refcountable.h>

namespace render
{

// Параметры вершинного буфера.
struct VertexBufferParameters
{
    // Формат вершинного буфера.
    u32 fvf;
};

// Базовый класс для вершинных буферов.
// Данные о вершинах могут храниться как в видеопамяти,
// так и в оперативной (для DrawPrimitiveUP).
class VertexBufferResource : public utility::refcountable
{
public:
    virtual ~VertexBufferResource() = 0 {}
    
    // Параметры вершинного буфера.
    inline const VertexBufferParameters & parameters() const
    {
        return mParameters;
    }
    
private:
    const VertexBufferParameters mParameters;
    
    // Получение вершинного буфера. Если этот буфер находится в оперативной
    // памяти, возвращает NULL.
    virtual const LPDIRECT3DVERTEXBUFFER9 buffer() const = 0;
    // Получение указателя на данные вершинного буфера в оперативной памяти.
    // Если буфер аппаратный, бросает исключение.
    virtual const void * pointer() const = 0;
    
    // Подготовка к сбросу и восстановление после него.
    virtual void prepareToReset() = 0;
    virtual void restoreAfterReset() = 0;
    friend class VertexBufferManager;
    
protected:
    VertexBufferResource( const VertexBufferParameters & params )
        : mParameters(params)
        {}
};
typedef utility::reference<VertexBufferResource> VertexBuffer;


// Вершинный буфер в памяти пользователя.
class UserPointerVertexBufferResource : public VertexBufferResource
{
public:
    ~UserPointerVertexBufferResource();
    
    // TODO: переписать под RAII.
    void * lock();
    void unlock();

private:
    bool mIsLocked;
    void * mVertexData;
    
    const LPDIRECT3DVERTEXBUFFER9 buffer() const
    {
        return NULL;
    }
    const void * pointer() const
    {
        assert(NULL != mVertexData);
        return mVertexData;
    }
    void prepareToReset();
    void restoreAfterReset();
    
protected:
    UserPointerVertexBufferResource( const VertexBufferParameters & params,
                                     u32 vertexCount );
    friend class VertexBufferManager;
    
public:
    // TODO: убрать нафиг
    const void * deprecated_pointer_for_perversions() const
    {
        return pointer();
    }
};
typedef utility::reference<UserPointerVertexBufferResource> UserPointerVertexBuffer;


} // namespace render

#endif // _TANITA2_RENDER_VB_VBTYPES_H_
