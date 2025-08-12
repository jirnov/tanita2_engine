#include <precompiled.h>
#include "GameObject_.h"
#include "Application_.h"
#include "Sequence_.h"
//#include <tanita2/utility/singleton.h>
#include "sound/sound.h"

void exportGameObject( void )
{
    using namespace bp;
    using namespace ingame;

    enum_<ResourceType>("ResourceType")
        .value("RESOURCE_TYPE_PNG", RESOURCE_TYPE_DDS)
        .value("RESOURCE_TYPE_WAV", RESOURCE_TYPE_WAV)
        .value("RESOURCE_TYPE_RGN", RESOURCE_TYPE_RGN)
        .value("RESOURCE_TYPE_PTH", RESOURCE_TYPE_PTH)
        .value("RESOURCE_TYPE_OGG", RESOURCE_TYPE_OGG)
        ;
    
    typedef Wrapper<GameObject> GameObjectWrap;
    class_<GameObjectWrap>("GameObject")
        .def("update",             &GameObject::update,  &GameObjectWrap::default_update)
        .def("begin_update",       &GameObject::begin_update)
        .def("update_children",    &GameObject::update_children)
        .def("update_sounds",      &GameObject::update_sounds)
        .def("end_update",         &GameObject::end_update)
        .def("_release",            &GameObject::release)
        
        .def("add_sound",           &GameObject::add_sound,           (arg("name"), arg("path")))
        .def("add_music",           &GameObject::add_music,           (arg("name"), arg("path")))
        .def("add_streaming_sound", &GameObject::add_streaming_sound, (arg("name"), arg("path")))
        
        .def_readwrite("objects",  &GameObject::objects)
        .def_readwrite("sounds",   &GameObject::sounds)
        .def_readwrite("position", &GameObject::position)
        .def_readwrite("scale",    &GameObject::scale)
        .def_readwrite("rotation", &GameObject::rotation)
        
        .add_property("absolute_position", &GameObject::get_absolute_position)
        .add_property("absolute_rotation", &GameObject::get_absolute_rotation)
        
        .add_property("enable_effect",  &GameObject::get_enable_effect, &GameObject::set_enable_effect)
        
        .def("set_effect_float",    &GameObject::set_effect_float)
        .def("set_effect_int",      &GameObject::set_effect_int)
        .def("set_effect_matrix",   &GameObject::set_effect_matrix)
        .def("set_effect_vector",   &GameObject::set_effect_vector)
        .def("set_effect_vec2",     &GameObject::set_effect_vec2)
        .def("set_effect_sequence", &GameObject::set_effect_sequence)

        .def("set_effect_technique",    &GameObject::set_effect_technique)
        .def("load_effect",             &GameObject::load_effect, (arg("path"),
                                                                   arg("default_technique") = "Render"))
        ;
    
    // Binding DirectoryId
    class_<DirectoryId>("ResourceId", init<U32, ResourceType>())
        .def(init<const char *, ResourceType>())
        ;
    
    class_<GameObject::objects_dict, bases<tanita2_dict> >("objects_dict")
        .def("__setitem__",   &GameObject::objects_dict::setitem, with_custodian_and_ward<1,2>())
        ;

    class_<GameObject::objects_dict_with_nodes, bases<GameObject::objects_dict> >("objects_dict_with_nodes")
        .def("__setitem__",   &GameObject::objects_dict_with_nodes::setitem, with_custodian_and_ward<1,2>())
        ;
}

using namespace ingame;

// Constructor
GameObject::GameObject()
    : position(0, 0), rotation(0.0f), scale(1.0f, 1.0f),
      effect(NULL), effect_enabled(false), 
      param_worldview(0), param_proj(0), param_time(0), param_dt(0), 
      param_worldviewproj(0), param_texture(0),
      param_texture_size(0), effect_texture(NULL),
      effect_sequence(NULL), cached_effect_enabled(false), isok(true)
{
    objects.parent = this;
    
    D3DXMatrixTranslation(&transformation, -10000, -10000, 0);
    D3DXMatrixTranslation(&transformation_inv, 10000, 10000, 0);
    
    mNode = utility::singleton<render::SceneGraph>()->createNode();
}

GameObject::~GameObject()
{
    release();
}

void GameObject::release()
{
    if (!isok) return;
    
    objects.clear();
    sounds.clear();
    isok = false;
}

// Updating states of child objects
void GameObject::update( float dt )
{
    begin_update();
    update_children(dt);
    update_sounds(dt);
    end_update();
}

// Begin updating
void GameObject::begin_update()
{
    Direct3D d3d;
    
    // Rotation matrix
    D3DXMATRIX rotation_matrix;
    float angle = rotation * 3.1415926f / 180.0f;
    D3DXMatrixRotationZ(&rotation_matrix, angle);
    
    // Scaling matrix
    D3DXMATRIX scaling_matrix;
    D3DXMatrixScaling(&scaling_matrix, scale.x, scale.y, 1.0f);
    
    // Translation matrix
    D3DXMATRIX translation_matrix;
    D3DXMatrixTranslation(&translation_matrix, position.x, position.y, 0.0f);
    
    // Multiplying matrices
    D3DXMATRIX intermediate_matrix;
    D3DXMATRIX result_matrix;
    D3DXMatrixMultiply(&intermediate_matrix, &rotation_matrix, &translation_matrix);
    D3DXMatrixMultiply(&result_matrix, &scaling_matrix, &intermediate_matrix);
    
    // Pushing transformation to stack
    d3d->push_transform(result_matrix);
    transformation = d3d->get_transform();
    D3DXMatrixInverse(&transformation_inv, NULL, &transformation);

    if (effect_enabled)
    {
        utility::singleton<DeprecatedApplicationInstance> app;
        effect_begin->addToRenderQueue(app->application_dt / 1000.0f);
    }
    cached_effect_enabled = effect_enabled;
}

// Update children
void GameObject::update_children( float dt )
{
    tanita2_dict objects_copy(objects);
    for (tanita2_dict::iterator i = objects_copy.begin(); objects_copy.end() != i; ++i)
        ((GameObject &)(bp::extract<GameObject &>(i.value()))).update(dt);
}

// End updating
void GameObject::end_update()
{
    if (cached_effect_enabled)
    {
        utility::singleton<DeprecatedApplicationInstance> app;
        set_common_effect_parameters();
        effect_end->addToRenderQueue(app->application_dt / 1000.0f);
    }
    
    Direct3D d3d;
    d3d->pop_transform();
}

// Update sounds
void GameObject::update_sounds( float /* dt */ )
{
    tanita2_dict sounds_copy(sounds);
    for (tanita2_dict::iterator i = sounds_copy.begin(); sounds_copy.end() != i; ++i)
        ((SoundRef &)(bp::extract<SoundRef &>(i.value())))->addToQueue();
}

// Loading music
void GameObject::add_music( bp::str & name, const DirectoryId & path )
{
    SoundManager sm;
    sounds.setitem(name, bp::object(sm->loadSound(path.toResourceId())));
}

// Loading sound
void GameObject::add_sound( bp::str & name, const DirectoryId & path )
{
    SoundManager sm;
    sounds.setitem(name, bp::object(sm->loadSound(path.toResourceId())));
}

// Loading streaming sound
void GameObject::add_streaming_sound( bp::str & name, const DirectoryId & path )
{
    SoundManager sm;
    sounds.setitem(name, bp::object(sm->loadSound(path.toResourceId())));
}

// Get position in absolute coordinates
D3DXVECTOR2 GameObject::get_absolute_position() const
{
    D3DXVECTOR4 p_multiplied;
    D3DXVec2Transform(&p_multiplied, &D3DXVECTOR2(0, 0), &transformation);  
    return D3DXVECTOR2(p_multiplied.x, p_multiplied.y);
}

// Get absolute rotation angle
float GameObject::get_absolute_rotation() const
{
    D3DXVECTOR4 A, B;
    D3DXVec2Transform(&A, &D3DXVECTOR2(0, 0), &transformation);
    D3DXVec2Transform(&B, &D3DXVECTOR2(1, 0), &transformation);
    const float result = atan2f(B.y - A.y, B.x - A.x) * 180.0f / 3.1415926f;
    return result < 0 ? result + 360.0f : result;
}

// Load effect from file
void GameObject::load_effect( char * /* path */, char * /* default_technique */ )
{/*
    // Loading effect
    EffectsManager em;
    if (NULL == (effect = em->load(path)))
        return;
    
    // Obtaining parameter handles
    param_worldview     = effect->GetParameterBySemantic(NULL, "WORLDVIEW");
    param_proj          = effect->GetParameterBySemantic(NULL, "PROJECTION");
    param_worldviewproj = effect->GetParameterBySemantic(NULL, "WORLDVIEWPROJ");

    param_time    = effect->GetParameterBySemantic(NULL, "TIME");
    param_dt      = effect->GetParameterBySemantic(NULL, "TIME_DELTA");
    param_texture = effect->GetParameterBySemantic(NULL, "TEXTURE");

    param_texture_size = effect->GetParameterBySemantic(NULL, "TEXTURE_SIZE");
    
    // Creating render-to-target sequence closures
    SequenceManagerSingleton sm;
    BeginRenderTargetSequence * begin_seq = new BeginRenderTargetSequence();
    effect_begin = sm->registerSequence(begin_seq);
    effect_end = sm->registerSequence(new EndRenderTargetSequence(effect_begin, effect, param_texture));
    effect_texture = begin_seq->texture->get_texture();
    effect_sequence = dynamic_cast<BeginRenderTargetSequence *>(effect_begin.get());
    
    // Setting default technique
    set_effect_technique(default_technique);*/
}

// Sequence rendering with effects
void GameObject::set_effect_technique( char * technique )
{/*
    if (!effect) return;
    ASSERT_DIRECTX(effect->SetTechnique(technique));
    effect_enabled = true;*/
}

// Set common effect parameters
void GameObject::set_common_effect_parameters()
{/*
    Application app;
    Direct3D d3d;
    
#define SET_PARAM(Type, handle, value)  if (handle) effect->Set##Type(handle, value)

    SET_PARAM(Matrix, param_worldview,     &d3d->get_view_matrix());
    SET_PARAM(Matrix, param_proj,          &d3d->get_projection_matrix());
    SET_PARAM(Matrix, param_worldviewproj, &(d3d->get_view_matrix() * d3d->get_projection_matrix()));
    
    SET_PARAM(Float, param_time, app->application_time / 1000.0f);
    SET_PARAM(Float, param_dt,   app->application_dt / 1000.0f);
    
    SET_PARAM(Vector, param_texture_size, &effect_sequence->size);

#undef SET_PARAM*/
}

// Setting effect parameters
void GameObject::set_effect_float( const char * param_name, float value )
{
    if (!effect) return;
    effect->SetFloat(effect->GetParameterByName(NULL, param_name), value);
}

// Setting effect parameters
void GameObject::set_effect_int( const char * param_name, int value )
{
    if (!effect) return;
    effect->SetInt(effect->GetParameterByName(NULL, param_name), value);
}

// Setting effect parameters
void GameObject::set_effect_matrix( const char * param_name, const bp::tuple & matrix )
{
    if (!effect) return;
    D3DXMATRIX m;
    for (int i = 0; i < matrix.attr("__len__") && i < 16; ++i)
        m[i] = bp::extract<float>(matrix[i]);
    effect->SetMatrix(effect->GetParameterByName(NULL, param_name), &m);
}

// Setting effect parameters
void GameObject::set_effect_vector( const char * param_name, const bp::tuple & v )
{
    if (!effect) return;
    D3DXVECTOR4 vec;
    for (int i = 0; i < v.attr("__len__") && i < 4; ++i)
        vec[i] = bp::extract<float>(v[i]);
    effect->SetVector(effect->GetParameterByName(NULL, param_name), &vec);
}

// Setting effect parameters
void GameObject::set_effect_vec2( const char * param_name, const D3DXVECTOR2 & v )
{
    if (!effect) return;
    D3DXVECTOR4 vec(v.x, v.y, 0, 0);
    effect->SetVector(effect->GetParameterByName(NULL, param_name), &vec);
}

// Setting effect parameters
void GameObject::set_effect_sequence( const char * param_name, const char * size_param_name,
                                      int index, const graphics::SequenceRef & /* seq */ )
{
    if (!effect) return;
    TextureManager tm;
    //TextureRef texture = ((graphics::SequenceRef)seq).get_texture();
    throw Exception("");
    /*
    ASSERT(0 != index);
    TRY(texture->bind(index));
    
    DDSURFACEDESC2 & sd = texture->get_description();
    D3DXVECTOR4 size((float)sd.dwWidth, (float)sd.dwHeight, 0, 0);
    effect->SetVector(effect->GetParameterByName(NULL, size_param_name), &size);
    effect->SetTexture(effect->GetParameterByName(NULL, param_name), texture->get_texture());
    */
}

// Adding/changing child object
void GameObject::objects_dict::setitem( bp::str & index, bp::object & value )
{
    python::Python py;
    value.attr("parent") = py["weakref"].attr("proxy")(bp::ptr(parent));
    /*
    make_function(&objects_dict::get_parent,
                               return_internal_reference<>())(this));
    */
    value.attr("__dict__")["name"] = index;
    bp::object constructor = py->getattr(value, "construct");
    if (bp::object() != constructor)
        constructor();
    tanita2_dict::setitem(index, value);
}

// Adding/changing child object
void GameObject::objects_dict_with_nodes::setitem( bp::str & index, bp::object & value )
{
    objects_dict::setitem(index, value);
    GameObject & go = bp::extract<GameObject &>(value);
    
    assert(go.mNode);
    if (go.mNode->hasParent())
        go.mNode->parent()->detachNode(go.mNode);
    parent->mNode->attachNode(go.mNode);
}
