/*
 ������ "������2".
 ���������� ����� �����.
 */
#include <precompiled.h>
#include <tanita2/render/device.h>
#include <tanita2/render/scene.h>

// �����������.
render::NodeInstance::NodeInstance()
    : mParent(NULL),
      mPosition(0, 0, 0),
      mBoundingBoxW(0), mBoundingBoxH(0), mBoundingBoxD(0)
{
    D3DXMatrixIdentity(&mTransformation);
}

// ������������� ����.
render::Node render::NodeInstance::attachNode( render::Node node )
{
    // ���� ���� ����, ������� ���. ����� ��������� �� ���������� ������� ��������.
    if (!node)
        node = utility::singleton<SceneGraph>()->createNode();
    else
    {
        if (node->hasParent())
            throw std::logic_error("Node being attached already has parent.");
    }
    mChildren.push_back(node);
    node->mParent = WeakNode(this);
    return node;
}

// ������������ ��������� ����.
void render::NodeInstance::detachNode( render::Node node )
{
    if (!node || this != &*node->parent())
        throw std::logic_error("Node being detached is empty or not attached.");
    
    NodeList::const_iterator where = std::find(mChildren.begin(), mChildren.end(), node);
    assert(mChildren.end() != where); // ���� this == &*node->parent(), ���� ����� ���� � ������.
    mChildren.erase(where);
    node->mParent = WeakNode();
}

// �����������.
render::SceneGraph::SceneGraph()
{
    mRoot = createNode();
}

// �������� �������, �� � ���� �� ������������ ����.
render::Node render::SceneGraph::createNode()
{
    return Node(new NodeInstance());
}
