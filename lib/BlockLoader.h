
#ifndef KUN__BLOCKLOADER_H_INCLUDE
#define KUN__BLOCKLOADER_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


kvs::FilterBase.( ())
namespace kun
{

class BlockLoader : public kvs::FilterBase, public kvs::UnstructuredVolumeObject
{
    // Class name.
    kvsClassName( kvs::BlockLoader );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::UnstructuredVolumeObject );
    
    //block_size
    size_t block_size;
    size_t nx;
    size_t ny;
    size_t nz;
    float* ori_values;

public:

    BlockLoader( std::string filename );

public:

    kvs::ObjectBase* exec( const kvs::ObjectBase* object );
};

} // end of namespace kun

#endif // KVS__CUBE_TO_TETRAHEDRA_H_INCLUDE