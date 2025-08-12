/*
 ������ "������2".
 ��� - �������� ���������, ����������� ������, ��������� �� ���� DIP.
 */
#pragma once
#ifndef _TANITA2_RENDER_MESH_H_
#define _TANITA2_RENDER_MESH_H_

#include <tanita2/render/material.h>
#include <boost/noncopyable.hpp>

namespace render
{


// �������� ���������, ����������� ������, ��������� �� ���� DIP.
// TODO: ����������� �� Resource.
class MeshResource : boost::noncopyable
{
public:
    virtual ~MeshResource();
    
    
private:
    Material mMaterial;
};
typedef std::tr1::shared_ptr<MeshResource> Mesh;


} // namespace render

#endif // _TANITA2_RENDER_MESH_H_
