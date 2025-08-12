/*
 Движок "Танита2".
 Граф сцены.
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

// Узел графа сцены.
// К узлу может быть привязан меш - единица отрисовки.
typedef utility::reference<class NodeInstance> Node;
typedef utility::weakreference<class NodeInstance> WeakNode;
class NodeInstance : public utility::refcountable
{
public:
    // Установка преобразования узла (координат, поворота, масштаба).
    void transform( const D3DXVECTOR3 & position );
    void transform( const D3DXVECTOR3 & position, const D3DXVECTOR3 & scale );
    void transform( const D3DXVECTOR3 & position, const D3DXQUATERNION & rotation );
    void transform( const D3DXVECTOR3 & position, const D3DXQUATERNION & rotation,
                    const D3DXVECTOR3 & scale );
    
    // Присоединение меша.
    void attachMesh( const Mesh & mesh );
    
    // Присоединение дочернего узла. Этот узел не должен иметь родителя.
    // Если задан пустой узел, создает новый.
    Node attachNode( Node node = Node() );
    
    // Отсоединение дочернего узла. Узел должен быть в списке детей.
    void detachNode( Node node );
    
    
    // Проверка на наличие родительского узла.
    inline bool hasParent() const
    {
        return mParent;
    }
    // Проверка на наличие детей.
    inline bool hasChildren() const
    {
        return !mChildren.empty();
    }
    
    
    // Получение родительского узла.
    inline WeakNode parent() const
    {
        return mParent;
    }
    
    
private:
    // Родительский узел.
    WeakNode mParent;
    // Материал, используемый при отрисовке присоединенного меша.
    // Инициализируется копией материала меша.
    Material mMaterial;
    // Позиция центра узла относительно его родителя (с учетом трансформаций узла).
    D3DXVECTOR3 mPosition;
    // Bounding box узла (если в узле нет меша, то BB пуст).
    f32 mBoundingBoxW, mBoundingBoxH, mBoundingBoxD;
    // Матрица трансформации узла относительно родителя.
    D3DXMATRIX mTransformation;
    // Дочерние узлы.
    typedef std::vector<Node> NodeList;
    NodeList mChildren;
    
    
    // Конструктор.
    NodeInstance();
    friend class SceneGraph;
};


// Граф сцены - менеджер отрисовываемых объектов.
class SceneGraph : boost::noncopyable
{
public:
    // Получение корневого узла.
    inline Node & root()
    {
        return mRoot;
    }
    
    // Создание пустого, ни к чему не привязанного узла.
    Node createNode();
    
    
private:
    SceneGraph();
    friend class utility::manual_singleton<SceneGraph>;
    
    // Корневой элемент.
    Node mRoot;
};

} // namespace render

#endif // _TANITA2_RENDER_SCENE_H_
