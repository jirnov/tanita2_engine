#include <precompiled.h>
#include "LayerImage_.h"
#include "StaticSequence_.h"
//#include "Sequence_.h"

void exportLayerImage( void )
{
    using namespace bp;
    using namespace ingame;

    typedef Wrapper<LayerImage> LayerImageWrap;
    class_<LayerImageWrap, bases<GameObject> >("LayerImage")
        .def("_release",            &LayerImage::release)
        .def("update",              &LayerImage::update,  &LayerImageWrap::default_update)
        .def("load_image",          &LayerImage::load_image, (arg("path"), arg("compressed") = true))
        .def("direct_load_image",   &LayerImage::direct_load_image, (arg("path"), arg("compressed") = true))
        .def_readwrite("sequence",  &LayerImage::sequence)
        ;
}

using namespace ingame;

void LayerImage::release()
{
    if (!isok) return;
    
    sequence = graphics::SequenceRef();
    GameObject::release();
}

// Load image
void LayerImage::load_image( const DirectoryId & path, bool compressed )
{
    SequenceManagerSingleton sm;
    sequence = sm->registerSequence(new StaticSequence(path, compressed));
}

// Load image from direct path
void LayerImage::direct_load_image( const char * path, bool compressed )
{
    SequenceManagerSingleton sm;
    sequence = sm->registerSequence(new StaticSequence(DirectoryId(path, RESOURCE_TYPE_PNG), compressed));
}

// Rendering image
void LayerImage::update( float dt )
{
    begin_update();
    sequence->addToRenderQueue(dt);
    end_update();
}
