#include <precompiled.h>
#include "AnimatedObject_.h"

#include "State_.h"
#include "Sequence_.h"
#include "VideoSequence_.h"

#include "LargeAnimatedSequence_.h"
#include "LargeAnimatedSoundSequence_.h"

void exportAnimatedObject( void )
{
    using namespace bp;
    using namespace ingame;

    typedef Wrapper<AnimatedObject> AnimatedObjectWrap;
    class_<AnimatedObjectWrap, bases<GameObject> >("AnimatedObject")
        .def("_release",             &AnimatedObject::release)
        .def("update",               &AnimatedObject::update,  &AnimatedObjectWrap::default_update)

        .def("add_sequence",             &AnimatedObject::add_large_sequence,
            (arg("name"), arg("path"), arg("frames"), arg("compressed") = true))

        .def("add_large_sequence",       &AnimatedObject::add_large_sequence,
            (arg("name"), arg("path"), arg("frames"), arg("compressed") = true))
        
        .def("add_sound_sequence",       &AnimatedObject::add_large_sound_sequence,
            (arg("name"), arg("path"), arg("frames"), arg("sounds"), arg("compressed") = true))

        .def("add_large_sound_sequence", &AnimatedObject::add_large_sound_sequence,
            (arg("name"), arg("path"), arg("frames"), arg("compressed") = true))
        
        .def("add_video_sound_sequence", &AnimatedObject::add_video_sound_sequence,
            (arg("name"), arg("path")))
            
        .def_readwrite("sequences",         &AnimatedObject::sequences)
        .def_readwrite("states",            &AnimatedObject::states)
        .def_readwrite("is_inside_zregion", &AnimatedObject::is_inside_zregion)
        
        .add_property("state", &AnimatedObject::get_state, &AnimatedObject::set_state)
        ;
}

using namespace ingame;

void AnimatedObject::release()
{
    if (!isok) return;
    
    sequences.clear();
    states.clear();
    
    GameObject::release();
}


// Set state
void AnimatedObject::set_state( bp::str & next_state )
{
    if (bp::str() != next_state && state != next_state)
    {
        // Calling on_exit of previous state
        if (bp::str() != state)
        {
            State & old_state = bp::extract<State &>(states[state]);
            old_state.on_exit();
        }
        
        // Changing state
        state = next_state;
                
        // Calling on_enter of current state
        State & new_state = bp::extract<State &>(states[state]);
        // Starting to play current sequence
        if (bp::object() != new_state.sequence)
        {
            sequences[bp::str(new_state.sequence)].attr("frame") = 0;
            sequences[bp::str(new_state.sequence)].attr("play")();
        }
        new_state.on_enter();
    }
}

// Loading sequence
void AnimatedObject::add_large_sequence( bp::str & name, const DirectoryId & path, bp::tuple & frame_numbers,
                                         bool compressed )
{
    SequenceManagerSingleton sm;
    std::vector<int> frames;
    for (int i = 0; i < bp::extract<int>(frame_numbers.attr("__len__")()); ++i)
        frames.push_back(bp::extract<int>(frame_numbers[i]));
    sequences.setitem(name, bp::object(sm->registerSequence(new LargeAnimatedSequence(path,
                                                                frames, compressed))));
}

// Load large sequence with sound
void AnimatedObject::add_large_sound_sequence( bp::str & name, const DirectoryId & path,
                                               bp::tuple & frame_numbers, bp::tuple & sounds, bool compressed )
{
    SequenceManagerSingleton sm;
    std::vector<int> frames;
    for (int i = 0; i < bp::extract<int>(frame_numbers.attr("__len__")()); ++i)
        frames.push_back(bp::extract<int>(frame_numbers[i]));
    
    LargeAnimatedSoundSequence::FrameSounds index_sound;
    for (int i = 0; i < bp::extract<int>(sounds.attr("__len__")()); ++i)
        index_sound.push_back(std::make_pair(int(bp::extract<int>(sounds[i][0])), 
            (SoundRef)bp::extract<SoundRef>(this->sounds[bp::str(sounds[i][1])])));
    sequences.setitem(name,
        bp::object(sm->registerSequence(new LargeAnimatedSoundSequence(path,
                                                  frames, index_sound, compressed))));
}

// Load video sequence with sounds
void AnimatedObject::add_video_sound_sequence( bp::str & name, const DirectoryId & path )
{
    Direct3D d3d;
    SequenceManagerSingleton sm;
    ResourceId p(path.toResourceId());
    if (d3d->shaders_enabled)
        sequences.setitem(name, bp::object(sm->registerSequence(new ShaderVideoSequence((char *)p.getFilePath()))));
    else if (d3d->hardware_yuv_enabled)
        sequences.setitem(name, bp::object(sm->registerSequence(new HWVideoSequence((char *)p.getFilePath()))));
    else
        sequences.setitem(name, bp::object(sm->registerSequence(new CPUConversionVideoSequence((char *)p.getFilePath()))));
}

// Updating object state
void AnimatedObject::update( float dt )
{
    // If we are inside z-region, we should just update sequence's transformation matrix
    if (is_inside_zregion)
    {
        begin_update();
        State & current_state = bp::extract<State &>(states[state]);
        graphics::SequenceRef & s = bp::extract<graphics::SequenceRef &>(sequences[bp::str(current_state.sequence)]);
        s->updateTransformationMatrix();
        
        update_children(dt);
        end_update();
        is_inside_zregion = false;
        return;
    }
    
    begin_update();
    update_sounds(dt);
    State & current_state = bp::extract<State &>(states[state]);

    // Processing links
    if (bp::object ns = current_state.link())
        set_state(bp::str(ns));
    
    // Calling on_update function
    current_state.on_update(dt);
    
    // Playing current sequence
    if (current_state.sequence)
        sequences[bp::str(current_state.sequence)].attr("render")(dt);
    
    // Updating children
    GameObject::update_children(dt);
    end_update();
}
