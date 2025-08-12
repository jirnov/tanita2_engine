#pragma once

#include "config/config.h"

#include "base/templates.h"
#include "Exceptions_.h"

namespace render
{
    class Device;
}

namespace graphics
{
    class SequenceManager;
};

//! Rendering manager
class Direct3DInstance
{
public:
    struct initializer
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 device;
        D3DPRESENT_PARAMETERS params;
        render::Device * renderDevice;
    };
    //! Initializes Direct3D API and rendering infrastructure
    Direct3DInstance( const initializer & init );
    //! Direct3D cleanup
    ~Direct3DInstance();
    
    //! Updating graphics system
    void update( DWORD dt );
    
    //! Clear screen
    /** @param  color  color to clear */
    void clear( D3DCOLOR color );
    //! Present all geometry and flip buffers 
    void present();

    //! Get projection matrix
    inline const D3DXMATRIX & get_projection_matrix() const
        { return matrix_projection; }
    //! Get view matrix
    inline const D3DXMATRIX & get_view_matrix() const
        { return matrix_view; }

    //! Multiply current world matrix and push to stack
    /** @param  new_transform  transformation to apply */
    void push_transform( const D3DXMATRIX & new_transform );
    //! Pop transformation matrix from stack
    void pop_transform();
    //! Get current transformation matrix (matrix stack top)
    D3DXMATRIX get_transform();
    
    //
    inline D3DXMATRIX getTransformationMatrix() { return get_transform(); }
    inline D3DXMATRIX getInverseTransformationMatrix()
        { D3DXMATRIX m;
          D3DXMATRIX tr = get_transform();
          D3DXMatrixInverse(&m, NULL, &tr);
          return m; }

	// Flag indicating that we need to save screenshot
	void save_screenshot( char * filename, int width, int height );
    
    // Texture memory manager
    class TextureManagerInstance;
    //! TextureManagerInstance singleton definition
    typedef utility::singleton<TextureManagerInstance> TextureManager;
    
    // Effects manager
    class EffectsManagerInstance;
    //! TextureManagerInstance singleton definition
    typedef utility::singleton<EffectsManagerInstance> EffectsManager;
    
    //! Text renderer
    LPD3DXFONT text_drawer;
    
    //! View and projection matrix setup
    void setup_matrices();

    //! Device capabilities
    D3DCAPS9 device_caps;
    //! Render-to-texture support flag
    bool rtt_enabled;
    //! Hardware YUV conversion support flag
    bool hardware_yuv_enabled; 
    
    bool shaders_enabled;

	//! Flag indicating that we need to save screenshot
	static bool need_save_screenshot;
    
    // Default render target
    std::vector<LPDIRECT3DSURFACE9> default_render_target;

protected:

    //! Direct3D object
    LPDIRECT3D9 d3d;
    //! Texture manager
    utility::manual_singleton<TextureManagerInstance> texture_manager;
    //! Effects manager
    utility::manual_singleton<EffectsManagerInstance> effects_manager;
    
    //! Animation sequence rendering manager
    utility::manual_singleton<graphics::SequenceManager> sequence_manager;
    
    //! Render state setup
    void setup_renderstate();
    
    //! Near and far clipping planes
    static const int ZNear = 1, ZFar = 1000;
    //! Matrix stack
    LPD3DXMATRIXSTACK matrix_stack;

	//! Values for saving  screenshot
	std::string screenshot_name;
	int screenshot_width;
	int screenshot_height;
    
    //! Projection matrix
    D3DXMATRIX matrix_projection;
    //! View matrix
    D3DXMATRIX matrix_view;
    
    //! Flag indicating that device was lost
    bool is_device_lost;
    
public:
    //! Check if device is lost and can be restored
    bool is_lost();

    //! Device object
    LPDIRECT3DDEVICE9 device;
    
    //! Clear rendering queue
    /** Used in on_script_reload function of DeprecatedApplicationInstance */
    void clear_render_queue();
};
//! Direct3D singleton definition
typedef utility::singleton<Direct3DInstance> Direct3D;

//! Texture manager
typedef Direct3DInstance::TextureManager TextureManager;
//! Effects rendering manager
typedef Direct3DInstance::EffectsManager EffectsManager;
