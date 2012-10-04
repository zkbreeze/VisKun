/*****************************************************************************/
/**
 *  @file   StructuredVolumeImporter.h
 *  @author Naohisa Sakamoto
 *  @brief  
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
#ifndef UTIL__STRUCTURED_VOLUME_IMPORTER_H_INCLUDE
#define UTIL__STRUCTURED_VOLUME_IMPORTER_H_INCLUDE

#include <kvs/ImporterBase>
#include <kvs/PolygonObject>
#include <kvs/StructuredVolumeObject>
#include <kvs/GrADS>


namespace util
{

class StructuredVolumeImporter : public kvs::ImporterBase, public kvs::StructuredVolumeObject
{
public:

    StructuredVolumeImporter(
        const std::string& filename,
        const size_t vindex = 0,
        const size_t tindex = 0,
        const bool zflip = false,
        const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform );

    StructuredVolumeImporter(
        const kvs::GrADS* file,
        const size_t vindex = 0,
        const size_t tindex = 0,
        const bool zflip = false,
        const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform );

public:

    kvs::StructuredVolumeObject* exec( const kvs::FileFormatBase* file );

private:

    void import(
        const kvs::GrADS* file,
        const size_t vindex,
        const size_t tindex,
        const bool zflip,
        const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform );
};

} // end of namespace util

#endif // UTIL__STRUCTURED_VOLUME_IMPORTER_H_INCLUDE
