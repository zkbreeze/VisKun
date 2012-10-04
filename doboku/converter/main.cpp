#include <iostream>
#include <fstream>
#include <string>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeExporter>
#include <kvs/KVSMLObjectStructuredVolume>


kvs::StructuredVolumeObject* ReadVelocityData(
    const char* data_filename,
    const char* coord_filename )
{
    size_t dimx, dimy, dimz;
    std::ifstream coord_file( coord_filename );
    coord_file >> dimx >> dimy >> dimz;
    coord_file.close();

    double* udata = new double [ dimx * dimy * dimz ];
    double* vdata = new double [ dimx * dimy * dimz ];
    double* wdata = new double [ dimx * dimy * dimz ];
    std::ifstream data_file( data_filename );
    data_file.read( (char*)udata, dimx * dimy * dimz * sizeof(double) );
    data_file.read( (char*)vdata, dimx * dimy * dimz * sizeof(double) );
    data_file.read( (char*)wdata, dimx * dimy * dimz * sizeof(double) );
    data_file.close();

    float* data = new float [ dimx * dimy * dimz ];
    for ( size_t i = 0; i < dimx * dimy * dimz; i++ )
    {
        double u = udata[i];
        double v = vdata[i];
        double w = wdata[i];
        data[i] = static_cast<float>( std::sqrt( u * u + v * v + w * w ) );
    }
    delete [] udata;
    delete [] vdata;
    delete [] wdata;

    kvs::Vector3ui resolution( dimx, dimy, dimz );
    kvs::ValueArray<float> values( data, dimx * dimy * dimz );
    kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
    object->setVeclen( 1 );
    object->setGridType( kvs::StructuredVolumeObject::Uniform );
    object->setResolution( resolution );
    object->setValues( kvs::AnyValueArray( values ) );
    object->updateMinMaxCoords();
    object->updateMinMaxValues();

    delete [] data;

    return object;
}

kvs::StructuredVolumeObject* ReadPressureData(
    const char* data_filename,
    const char* coord_filename )
{
    size_t dimx, dimy, dimz;
    std::ifstream coord_file( coord_filename );
    coord_file >> dimx >> dimy >> dimz;
    coord_file.close();

    float* data = new float [ dimx * dimy * dimz ];
    std::ifstream data_file( data_filename );
    data_file.seekg( dimx * dimy * dimz * sizeof(double), std::ios::beg );
    data_file.seekg( dimx * dimy * dimz * sizeof(double), std::ios::cur );
    data_file.seekg( dimx * dimy * dimz * sizeof(double), std::ios::cur );
    for ( size_t i = 0; i < dimx * dimy * dimz; i++ )
    {
        double temp = 0.0;
        data_file.read( (char*)&temp, sizeof(double) );
        data[i] = static_cast<float>( temp );
    }
    data_file.close();

    kvs::Vector3ui resolution( dimx, dimy, dimz );
    kvs::ValueArray<float> values( data, dimx * dimy * dimz );
    kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
    object->setVeclen( 1 );
    object->setGridType( kvs::StructuredVolumeObject::Uniform );
    object->setResolution( resolution );
    object->setValues( kvs::AnyValueArray( values ) );
    object->updateMinMaxCoords();
    object->updateMinMaxValues();

    delete [] data;

    return object;
}

void WriteData( kvs::StructuredVolumeObject* object, const char* filename )
{
    typedef kvs::KVSMLObjectStructuredVolume KVSMLData;
    typedef kvs::StructuredVolumeExporter<KVSMLData> Exporter;
    KVSMLData* kvsml = new Exporter( object );
    kvsml->setWritingDataType( KVSMLData::ExternalBinary );
    kvsml->write( filename );
    delete kvsml;
}

int main( int argc, char** argv )
{
    char* data_filename = argv[1];
    char* coord_filename = argv[2];
    kvs::StructuredVolumeObject* velocity = ReadVelocityData( data_filename, coord_filename );
    kvs::StructuredVolumeObject* pressure = ReadPressureData( data_filename, coord_filename );
    WriteData( pressure, argv[3] );
    WriteData( velocity, argv[4] );
    delete pressure;
    delete velocity;

    return 0;
}
