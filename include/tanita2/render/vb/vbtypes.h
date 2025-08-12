/*
 ������ "������2".
 �������� ��������� �������.
 */
#pragma once
#ifndef _TANITA2_RENDER_VB_VBTYPES_H_
#define _TANITA2_RENDER_VB_VBTYPES_H_

#include <boost/noncopyable.hpp>
#include <tanita2/utility/refcountable.h>

namespace render
{

// ��������� ���������� ������.
struct VertexBufferParameters
{
    // ������ ���������� ������.
    u32 fvf;
};

// ������� ����� ��� ��������� �������.
// ������ � �������� ����� ��������� ��� � �����������,
// ��� � � ����������� (��� DrawPrimitiveUP).
class VertexBufferResource : public utility::refcountable
{
public:
    virtual ~VertexBufferResource() = 0 {}
    
    // ��������� ���������� ������.
    inline const VertexBufferParameters & parameters() const
    {
        return mParameters;
    }
    
private:
    const VertexBufferParameters mParameters;
    
    // ��������� ���������� ������. ���� ���� ����� ��������� � �����������
    // ������, ���������� NULL.
    virtual const LPDIRECT3DVERTEXBUFFER9 buffer() const = 0;
    // ��������� ��������� �� ������ ���������� ������ � ����������� ������.
    // ���� ����� ����������, ������� ����������.
    virtual const void * pointer() const = 0;
    
    // ���������� � ������ � �������������� ����� ����.
    virtual void prepareToReset() = 0;
    virtual void restoreAfterReset() = 0;
    friend class VertexBufferManager;
    
protected:
    VertexBufferResource( const VertexBufferParameters & params )
        : mParameters(params)
        {}
};
typedef utility::reference<VertexBufferResource> VertexBuffer;


// ��������� ����� � ������ ������������.
class UserPointerVertexBufferResource : public VertexBufferResource
{
public:
    ~UserPointerVertexBufferResource();
    
    // TODO: ���������� ��� RAII.
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
    // TODO: ������ �����
    const void * deprecated_pointer_for_perversions() const
    {
        return pointer();
    }
};
typedef utility::reference<UserPointerVertexBufferResource> UserPointerVertexBuffer;


} // namespace render

#endif // _TANITA2_RENDER_VB_VBTYPES_H_
