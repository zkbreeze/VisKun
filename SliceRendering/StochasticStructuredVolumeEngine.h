/*****************************************************************************/
/**
 *  @file   StochasticPolygonEngine.h
 *  @author Jun Nishimura
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#ifndef KVS__GLEW__STOCHASTIC_STRUCTURED_VOLUME_ENGINE_H_INCLUDE
#define KVS__GLEW__STOCHASTIC_STRUCTURED_VOLUME_ENGINE_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/StructuredVolumeObject>
#include <kvs/glew/VertexBufferObject>
#include <kvs/glew/IndexBufferObject>
#include <kvs/TransferFunction>
#include "StochasticRenderingEngine.h"
#include <kvs/Xorshift128>


namespace kvs
{

class TransferFunction;

namespace glew
{

/*===========================================================================*/
/**
 *  @brief  Stochastic rendering engine class for polygon object.
 */
/*===========================================================================*/
class StochasticStructuredVolumeEngine : public kvs::glew::StochasticRenderingEngine
{
    // Class name.
    kvsClassName( kvs::glew::StochasticStructuredVolumeEngine );

protected:

    typedef kvs::glew::StochasticRenderingEngine BaseClass;

private:

    //const kvs::PolygonObject* m_ref_polygon; ///< pointer to a polygon object (reference only)
    const kvs::StructuredVolumeObject* m_ref_volume;
    size_t m_loc_identifier; ///< location indentifier for VBO
    GLuint m_texture_name;
    GLuint m_transfer_function_name;
    kvs::TransferFunction m_transfer_function;
    kvs::Xorshift128 m_rng;

public:

    StochasticStructuredVolumeEngine();

public:

    void initialize();

    void test_setVolume(const kvs::StructuredVolumeObject* volume)
    {
        m_ref_volume = volume;
    }

public:

    const kvs::ObjectBase* object() const;

    const EngineType engineType() const;

    void attachObject( const kvs::ObjectBase* object );

    void clearEnsembleBuffer();

    void setTransferFunction(const kvs::TransferFunction&);

private:

    void setup_shader( const float modelview_matrix[16] );

    void initialize_shader();

    void create_vertex_buffer(){}

    void download_vertex_buffer(){}

    void draw_vertex_buffer( const float modelview_matrix[16] );
};

} // end of namespace glew

} // end of namespace kvs

#endif // KVS__GLEW__STOCHASTIC_STRUCTURED_VOLUME_ENGINE_H_INCLUDE
