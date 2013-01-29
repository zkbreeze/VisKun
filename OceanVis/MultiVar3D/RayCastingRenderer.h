/*****************************************************************************/
/**
 *  @file   RayCastingRenderer.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: RayCastingRenderer.h 1149 2012-06-02 13:27:56Z s.yamada0808@gmail.com $
 */
/*****************************************************************************/
#ifndef KVS__GLEW__RAY_CASTING_RENDERER_H_INCLUDE
#define KVS__GLEW__RAY_CASTING_RENDERER_H_INCLUDE

#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/VolumeRendererBase>
#include <kvs/Texture1D>
#include <kvs/Texture2D>
#include <kvs/ObjectBase>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/StructuredVolumeObject>
#include <kvs/glew/Texture3D>
#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/ProgramObject>
#include <kvs/glew/ShaderSource>
#include <kvs/glew/FrameBufferObject>
#include "TransferFunction3D.h"

namespace kun
{
//#define KVS_GLEW_RAY_CASTING_RENDERER__EMBEDDED_SHADER

/*===========================================================================*/
/**
 *  @brief  RayCastingRenderer class.
 */
/*===========================================================================*/
class RayCastingRenderer : public kvs::VolumeRendererBase
{
    // Class name.
    kvsClassName( kvs::glew::RayCastingRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::VolumeRendererBase );

public:

    enum DrawingBuffer
    {
        FrontFace,
        BackFace,
        Volume
    };

protected:

    bool m_draw_front_face; ///< frag for drawing front face
    bool m_draw_back_face; ///< frag for drawing back face
    bool m_draw_volume; ///< frag for drawing volume
    bool m_enable_jittering; ///< frag for stochastic jittering
    float m_step; ///< sampling step
    float m_opaque; ///< opaque value for early ray termination
    kvs::Texture1D m_transfer_function_texture; ///< transfer function texture
    kvs::Texture2D m_jittering_texture; ///< texture for stochastic jittering
    kvs::Texture2D m_entry_points; ///< entry point texture
    kvs::Texture2D m_exit_points; ///< exit point texture
    kvs::glew::FrameBufferObject m_entry_exit_framebuffer; ///< framebuffer object for entry/exit point texture
    kvs::glew::VertexBufferObject m_bounding_cube; ///< bounding cube (VBO)
    kvs::glew::Texture3D m_volume_data; ///< volume data (3D texture)
    kvs::glew::ProgramObject m_ray_caster; ///< ray casting shader
    kvs::glew::ProgramObject m_bounding_cube_shader; ///< bounding cube shader
    kvs::Texture2D m_color_texture;
    kvs::Texture2D m_depth_texture;
    
    kvs::glew::Texture3D m_3d_transfer_function_texture;///< 3d transfer function texture
    float* m_3d_tfunc_data;
    size_t m_3d_width;
    size_t m_3d_height;
    size_t m_3d_depth;
    kvs::StructuredVolumeObject* m_volume_2;
    kvs::glew::Texture3D m_volume_data_2;
    kvs::StructuredVolumeObject* m_volume_3;
    kvs::glew::Texture3D m_volume_data_3;

public:

    RayCastingRenderer( void );

    RayCastingRenderer( const kvs::TransferFunction& tfunc );

    template <typename ShadingType>
    RayCastingRenderer( const ShadingType shader );

    virtual ~RayCastingRenderer( void );

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

public:

    void initialize( void );

    void setSamplingStep( const float step );

    void setOpaqueValue( const float opaque );

    void setDrawingBuffer( const DrawingBuffer drawing_buffer );

    void setTransferFunction( const kvs::TransferFunction& tfunc );
    
    void set3DTransferFunction( kun::TransferFunction3D tfunc3d );

    void enableJittering( void );

    void disableJittering( void );
    
    void addVolume2( kvs::StructuredVolumeObject* volume );
    
    void addVolume3( kvs::StructuredVolumeObject* volume );

protected:

    void create_image(
        const kvs::StructuredVolumeObject* volume,
        const kvs::Camera* camera,
        const kvs::Light* light );

    void initialize_shaders( const kvs::StructuredVolumeObject* volume );

#if KVS_ENABLE_DEPRECATED
    void create_shaders(
        kvs::glew::ProgramObject& program_object,
        const kvs::glew::ShaderSource& vertex_source,
        const kvs::glew::ShaderSource& fragment_source );
#endif

    void create_entry_points( void );

    void create_exit_points( void );

    void create_jittering_texture( void );

    void create_bounding_cube( const kvs::StructuredVolumeObject* volume );
    
    void create_3d_transfer_function( void );

    void create_volume_data( const kvs::StructuredVolumeObject* volume, kvs::glew::Texture3D& volume_data );
    
    void draw_bounding_cube( void );

    void draw_quad( const float opacity );
};

} // end of namespace kun

#endif // KVS__GLEW__RAY_CASTING_RENDERER_H_INCLUDE
