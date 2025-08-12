/*
 ������ "������2".
 ���� �����.
 */
#pragma once
#ifndef _TANITA2_RENDER_SCENE_H_
#define _TANITA2_RENDER_SCENE_H_

#include <tanita2/utility/refcountable.h>
#include <tanita2/config/config.h>
#include <tanita2/render/mesh.h>
#include <boost/noncopyable.hpp>
#include <d3dx9math.h>

namespace render
{

// ���� ����� �����.
// � ���� ����� ���� �������� ��� - ������� ���������.
typedef utility::reference<class NodeInstance> Node;
typedef utility::weakreference<class NodeInstance> WeakNode;
class NodeInstance : public utility::refcountable
{
public:
    // ��������� �������������� ���� (���������, ��������, ��������).
    void transform( const D3DXVECTOR3 & position );
    void transform( const D3DXVECTOR3 & position, const D3DXVECTOR3 & scale );
    void transform( const D3DXVECTOR3 & position, const D3DXQUATERNION & rotation );
    void transform( const D3DXVECTOR3 & position, const D3DXQUATERNION & rotation,
                    const D3DXVECTOR3 & scale );
    
    // ������������� ����.
    void attachMesh( const Mesh & mesh );
    
    // ������������� ��������� ����. ���� ���� �� ������ ����� ��������.
    // ���� ����� ������ ����, ������� �����.
    Node attachNode( Node node = Node() );
    
    // ������������ ��������� ����. ���� ������ ���� � ������ �����.
    void detachNode( Node node );
    
    
    // �������� �� ������� ������������� ����.
    inline bool hasParent() const
    {
        return mParent;
    }
    // �������� �� ������� �����.
    inline bool hasChildren() const
    {
        return !mChildren.empty();
    }
    
    
    // ��������� ������������� ����.
    inline WeakNode parent() const
    {
        return mParent;
    }
    
    
private:
    // ������������ ����.
    WeakNode mParent;
    // ��������, ������������ ��� ��������� ��������������� ����.
    // ���������������� ������ ��������� ����.
    Material mMaterial;
    // ������� ������ ���� ������������ ��� �������� (� ������ ������������� ����).
    D3DXVECTOR3 mPosition;
    // Bounding box ���� (���� � ���� ��� ����, �� BB ����).
    f32 mBoundingBoxW, mBoundingBoxH, mBoundingBoxD;
    // ������� ������������� ���� ������������ ��������.
    D3DXMATRIX mTransformation;
    // �������� ����.
    typedef std::vector<Node> NodeList;
    NodeList mChildren;
    
    
    // �����������.
    NodeInstance();
    friend class SceneGraph;
};


// ���� ����� - �������� �������������� ��������.
class SceneGraph : boost::noncopyable
{
public:
    // ��������� ��������� ����.
    inline Node & root()
    {
        return mRoot;
    }
    
    // �������� �������, �� � ���� �� ������������ ����.
    Node createNode();
    
    
private:
    SceneGraph();
    friend class utility::manual_singleton<SceneGraph>;
    
    // �������� �������.
    Node mRoot;
};

} // namespace render

#endif // _TANITA2_RENDER_SCENE_H_
