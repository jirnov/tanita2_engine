#pragma once
#include <tanita2/render/scene.h>
#include "Graphics_.h"
#include "EffectsManager_.h"
#include "DirectoryId_.h"

// Forward declaration
void exportGameObject( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Base class for all ingame objects
class GameObject
{
public:
    //! Constructor
    GameObject();
    
    inline bool isOk() { return isok; }
    void release();
    
    //! Destructor
    virtual ~GameObject();
    
    //! Updating states of child objects
    /** This method calls begin_update,
      * then updates all children and sounds and
      * calls end_update.
      * \param  dt  time step */
    virtual void update( float dt );
    
    // Load effect from file
    void load_effect( char * path, char * default_technique );
    // Set effect technique
    void set_effect_technique( char * technique );

    //! Load sound
    void add_sound( bp::str & name, const DirectoryId & path );
    //! Load music
    void add_music( bp::str & name, const DirectoryId & path );
    //! Load streaming sound
    void add_streaming_sound( bp::str & name, const DirectoryId & path );
    
    //! Get position in absolute coordinates
    D3DXVECTOR2 get_absolute_position() const;
    //! Get absolute rotation (in degrees)
    float get_absolute_rotation() const;

protected:
    //! Class for automatic management of children classes
    class objects_dict: public tanita2_dict
    {
    public:
        // Constructor
        inline objects_dict() : parent(NULL) {}
        
        //! Adding/changing child object
        void setitem( bp::str & index, bp::object & value );
        
        // Parent object
        GameObject * parent;
        inline GameObject * get_parent() { return parent; }
    };
    
    class objects_dict_with_nodes: public objects_dict
    {
    public:
        void setitem( bp::str & index, bp::object & value );
    };
    
    bool isok;

    //! Begin updating (apply transformations)
    /** This method should be called before any 
      * updates to apply object transformations 
      * correctly. After updating end_update
      * should be called */
    void begin_update();
    //! End update (restore transformations)
    /** This method should be called at end of 
      * update to restore current transformations
      * modified for updating of this object */
    void end_update();
    //! Update child objects
    /** Should be called between begin_update and end_update
      * to ensure proper coordinates */
    void update_children( float dt );
    //! Update sounds
    /** Should be called between begin_update and end_update
      * to ensure proper coordinates */
    void update_sounds( float dt );
    
    // Set common effect parameters
    void set_common_effect_parameters();
    
    //! Enable/disable effect
    inline bool get_enable_effect() { return effect_enabled; }
    inline void set_enable_effect( bool state )
        { effect_enabled = effect && state; }
        
    // Setting effect parameters
    void set_effect_float( const char * param_name, float value );
    void set_effect_int( const char * param_name, int value );
    void set_effect_matrix( const char * param_name, const bp::tuple & matrix );
    void set_effect_vector( const char * param_name, const bp::tuple & v );
    void set_effect_vec2( const char * param_name, const D3DXVECTOR2 & v );
    void set_effect_sequence( const char * param_name, const char * size_param_name,
                              int index, const graphics::SequenceRef & seq );
    
    //! Position
    D3DXVECTOR2 position;
    //! Rotation angle (in degrees)
    float rotation;
    //! Scaling factor
    D3DXVECTOR2 scale;
    
    //! Child objects
    objects_dict_with_nodes objects;
    
    //! Cached transformation matrix
    D3DXMATRIX transformation;
    //! Inverse transformation matrix
    D3DXMATRIX transformation_inv;
    
    //! Sounds
    tanita2_dict sounds;
    
    // Handle to common parameters
    D3DXHANDLE param_worldview;
    D3DXHANDLE param_proj;
    D3DXHANDLE param_worldviewproj;
    D3DXHANDLE param_time;
    D3DXHANDLE param_dt;
    D3DXHANDLE param_texture;
    D3DXHANDLE param_texture_size;

    // Effect enabled flag
    bool effect_enabled;
    // Flag for proper begin-end closing (if ef_enabled changed between closures)
    bool cached_effect_enabled;

    // Effect handle
    EffectRef effect;
    
    // Effect render target sequence closures
    graphics::SequenceRef effect_begin, effect_end;
    // Render target texture
    LPDIRECT3DTEXTURE9 effect_texture;
    const BeginRenderTargetSequence * effect_sequence;
    
    render::Node mNode;
    
    // Friend class
    friend class Path;

private:
    friend void ::exportGameObject( void );
};

} // End of ::ingame namespace
