/*****************************************************************************/
/**
 *  @file   StochasticPolygonEngine.cpp
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
#include <GL/glew.h>
#include "StochasticStructuredVolumeEngine.h"
#include <kvs/TransferFunction>
#include <kvs/Timer>
#include <kvs/CellByCellParticleGenerator>
#include "float.h"

namespace
{
    const int NumberOfSlices = 100;
}

namespace kvs
{

namespace glew
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new StochasticPolygonEngine class.
 */
/*===========================================================================*/
StochasticStructuredVolumeEngine::StochasticStructuredVolumeEngine()
{
    this->initialize();
    BaseClass::setShader(kvs::Shader::Lambert());
}

/*===========================================================================*/
/**
 *  @brief  Initializes the member parameters.
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::initialize()
{
    m_ref_volume = NULL;
    m_loc_identifier = 0;
}

/*===========================================================================*/
/**
 *  @brief  Returns the pointer to the polygon object.
 *  @return pointer to the polygon object
 */
/*===========================================================================*/
const kvs::ObjectBase* StochasticStructuredVolumeEngine::object() const
{
    return m_ref_volume;
}

/*===========================================================================*/
/**
 *  @brief  Attaches a polygon object.
 *  @param  object [in] pointer to a polygon object
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::attachObject(const kvs::ObjectBase* object)
{
}

/*===========================================================================*/
/**
 *  @brief  Returns the rendering engine type.
 *  @return rendering engine type
 */
/*===========================================================================*/
const StochasticStructuredVolumeEngine::EngineType StochasticStructuredVolumeEngine::engineType() const
{
    return BaseClass::StructuredVolume;
}

/*===========================================================================*/
/**
 *  @brief  Clears the ensemble buffer.
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::clearEnsembleBuffer()
{
    m_repetition_count = 0;
}

/*===========================================================================*/
/**
 *  @brief  Sets parameters to the shaders.
 *  @param  modelview_matrix [in] modelview matrix
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::setup_shader(const float modelview_matrix[16])
{
    const size_t random_texture_size = m_random_texture.width();
    const float rp_x = (float)(m_rng.randInteger() % random_texture_size);
    const float rp_y = (float)(m_rng.randInteger() % random_texture_size);
    const GLfloat random_texture_size_inv = 1.0f / random_texture_size;
    const GLfloat screen_scale_x = m_width * 0.5f;
    const GLfloat screen_scale_y = m_height * 0.5f;

    m_shader_program.setUniformValuef("random_texture_size_inv", random_texture_size_inv);
    m_shader_program.setUniformValuef("random_offset", rp_x, rp_y);
    m_shader_program.setUniformValuef("screen_scale", screen_scale_x, screen_scale_y);
    m_shader_program.setUniformValuef("screen_scale_inv", 1.0f / m_width, 1.0f / m_height);
    m_shader_program.setUniformValuei("random_texture", 1);
    m_shader_program.setUniformValuei("volume_texture", 0);
    m_shader_program.setUniformValuei("transfer_function", 2);
}

/*===========================================================================*/
/**
 *  @brief  Initializes the shaders.
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::initialize_shader()
{
    const std::string vert_code = "polygon.vert";
    const std::string frag_code = "polygon.frag";

    kvs::glew::ShaderSource vert(vert_code);
    kvs::glew::ShaderSource frag(frag_code);

    if (BaseClass::isEnabledShading())
    {
        switch (BaseClass::m_shader->type())
        {
        case kvs::Shader::LambertShading: frag.define("ENABLE_LAMBERT_SHADING"); break;
        case kvs::Shader::PhongShading: frag.define("ENABLE_PHONG_SHADING"); break;
        case kvs::Shader::BlinnPhongShading: frag.define("ENABLE_BLINN_PHONG_SHADING"); break;
        default: /* NO SHADING */ break;
        }

        GLboolean status = 0; glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE, &status);
        if (status == GL_TRUE) { frag.define("ENABLE_TWO_SIDE_LIGHTING"); }
    }

    this->create_shaders(m_shader_program, vert, frag);
    m_loc_identifier = m_shader_program.attributeLocation("identifier");

    if (BaseClass::isEnabledShading())
    {
        m_shader_program.bind();
        switch (BaseClass::m_shader->type())
        {
        case kvs::Shader::LambertShading:
        {
            const GLfloat Ka = ((kvs::Shader::Lambert*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::Lambert*)(BaseClass::m_shader))->Kd;
            m_shader_program.setUniformValuef("shading.Ka", Ka);
            m_shader_program.setUniformValuef("shading.Kd", Kd);
            break;
        }
        case kvs::Shader::PhongShading:
        {
            const GLfloat Ka = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Kd;
            const GLfloat Ks = ((kvs::Shader::Phong*)(BaseClass::m_shader))->Ks;
            const GLfloat S  = ((kvs::Shader::Phong*)(BaseClass::m_shader))->S;
            m_shader_program.setUniformValuef("shading.Ka", Ka);
            m_shader_program.setUniformValuef("shading.Kd", Kd);
            m_shader_program.setUniformValuef("shading.Ks", Ks);
            m_shader_program.setUniformValuef("shading.S",  S);
            break;
        }
        case kvs::Shader::BlinnPhongShading:
        {
            const GLfloat Ka = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Ka;
            const GLfloat Kd = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Kd;
            const GLfloat Ks = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->Ks;
            const GLfloat S  = ((kvs::Shader::BlinnPhong*)(BaseClass::m_shader))->S;
            m_shader_program.setUniformValuef("shading.Ka", Ka);
            m_shader_program.setUniformValuef("shading.Kd", Kd);
            m_shader_program.setUniformValuef("shading.Ks", Ks);
            m_shader_program.setUniformValuef("shading.S",  S);
            break;
        }
        default: /* NO SHADING */ break;
        }
        m_shader_program.unbind();
    }

    if (glGetError() != GL_NO_ERROR) throw "Error";

    // enable 3D texture
    glEnable(GL_TEXTURE_3D);

    // init the 3D texture
    glGenTextures(1, &m_texture_name);
    glBindTexture(GL_TEXTURE_3D, m_texture_name);

    // texture environment setup (GL_CLAMP_TO_EDGE avoids invalid mapping at the texture border)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    kvs::Vector3ui res = m_ref_volume->resolution();
    unsigned char* pvalue = (unsigned char*)m_ref_volume->values().pointer();

      // load the texture image
    glTexImage3D(GL_TEXTURE_3D,   // target
        0,                        // level
        GL_RED,                   // internal format
        (int)res.x(),             // width
        (int)res.y(),             // height
        (int)res.z(),             // depth
        0,                        // border
        GL_RED,                   // format
        GL_UNSIGNED_BYTE,         // type
        pvalue);                  // buffer

    glBindTexture(GL_TEXTURE_3D, 0);

    const kvs::TransferFunction& tf = m_transfer_function;
    const size_t tres = tf.resolution();
    kvs::ValueArray<kvs::UInt8> rgba_array( tres * 4 );
    for (size_t i = 0; i < tres; i++)
    {
        rgba_array[i * 4 + 0] = tf.colorMap()[i].r();
        rgba_array[i * 4 + 1] = tf.colorMap()[i].g();
        rgba_array[i * 4 + 2] = tf.colorMap()[i].b();
        rgba_array[i * 4 + 3] = static_cast<kvs::UInt8>(tf.opacityMap()[i] * 255.5f);
    }

    glGenTextures(1, &m_transfer_function_name);
    glBindTexture(GL_TEXTURE_1D, m_transfer_function_name);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA,
        (int)tres,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rgba_array.data() );
    glBindTexture(GL_TEXTURE_1D, 0);
    if (glGetError() != GL_NO_ERROR) throw "Error";

}

/*===========================================================================*/
/**
 *  @brief  Draws the vertex buffer.
 *  @param  modelview_matrix [in] modelview matrix
 */
/*===========================================================================*/
void StochasticStructuredVolumeEngine::draw_vertex_buffer(const float modelview_matrix[16])
{
    static kvs::Timer timer;
    timer.start();

    glPushAttrib(GL_COLOR_BUFFER_BIT   |
                 GL_DEPTH_BUFFER_BIT   |
                 GL_ENABLE_BIT         |
                 GL_LIGHTING_BIT       |
                 GL_POLYGON_BIT        |
                 GL_TEXTURE_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_texture_name);
    glActiveTexture(GL_TEXTURE1);
    m_random_texture.bind();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, m_transfer_function_name);

    m_shader_program.bind();
    this->setup_shader(modelview_matrix);

    // openGL setup
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //--------------------------------------------------//
    // gets the direction of the observer
    double  gl_model[16];
    double  gl_proj [16];
    int     gl_view [4];
    glGetDoublev(GL_MODELVIEW_MATRIX , gl_model);
    glGetDoublev(GL_PROJECTION_MATRIX, gl_proj);
    glGetIntegerv(GL_VIEWPORT        , gl_view);

    //--------------------------------------------------//
    // gets the bounding box of the grid in the screen coordinates
    kvs::Vector3ui res = m_ref_volume->resolution();
    double xmin=FLT_MAX, xmax=-FLT_MAX, ymin=FLT_MAX, ymax=-FLT_MAX, zmin=FLT_MAX, zmax=-FLT_MAX;
    for (int i = 0; i < 8; ++i)
    {
        float bbx = (i&1) ? (float)res.x() : 0.0f;
        float bby = (i&2) ? (float)res.y() : 0.0f;
        float bbz = (i&4) ? (float)res.z() : 0.0f;

        double x,y,z;
        gluProject(bbx,bby,bbz, gl_model, gl_proj, gl_view, &x, &y, &z);

        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
        if (z < zmin) zmin = z;
        if (z > zmax) zmax = z;
    }

    //--------------------------------------------------//
    // world to tex coordinates
    double fx = 1.0 / res.x();
    double fy = 1.0 / res.y();
    double fz = 1.0 / res.z();

    //--------------------------------------------------//
    // draw each slice of the texture in the viewer coordinates
    int nslices = NumberOfSlices;
    float dz = (float)((zmax-zmin) / nslices);
    float z  = (float)zmax - dz/2.0f;

    const size_t random_texture_size = m_random_texture.width();

    for(int n = nslices-1; n >= 0; --n, z -= dz)
    {
        const float rp_x = (float)(m_rng.randInteger() % random_texture_size);
        const float rp_y = (float)(m_rng.randInteger() % random_texture_size);
        m_shader_program.setUniformValuef("random_offset", rp_x, rp_y);

        glBegin(GL_QUADS);
        GLdouble point[3];
        gluUnProject(xmin,ymin,z, gl_model, gl_proj, gl_view, point + 0, point + 1, point + 2);
        glTexCoord3d(fx*point[0], fy*point[1], fz*point[2]);
        glVertex3dv(point);

        gluUnProject(xmax,ymin,z, gl_model, gl_proj, gl_view, point + 0, point + 1, point + 2);
        glTexCoord3d(fx*point[0], fy*point[1], fz*point[2]);
        glVertex3dv(point);

        gluUnProject(xmax,ymax,z, gl_model, gl_proj, gl_view, point + 0, point + 1, point + 2);
        glTexCoord3d(fx*point[0], fy*point[1], fz*point[2]);
        glVertex3dv(point);

        gluUnProject(xmin,ymax,z, gl_model, gl_proj, gl_view, point + 0, point + 1, point + 2);
        glTexCoord3d(fx*point[0], fy*point[1], fz*point[2]);
        glVertex3dv(point);
        glEnd(); // GL_QUADS
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE1);
    m_random_texture.unbind();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, 0);
    glActiveTexture(GL_TEXTURE0);

    glDisable (GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_1D);
    m_shader_program.unbind();

    glPopAttrib();
    glFinish();
    timer.stop();
    std::cout << "\r" << timer.msec() << "[msec]";
}

void StochasticStructuredVolumeEngine::setTransferFunction(const kvs::TransferFunction& tf)
{
    m_transfer_function = tf;
}

} // end of namespace glew

} // end of namespace kvs
