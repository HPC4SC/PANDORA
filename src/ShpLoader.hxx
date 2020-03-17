
#ifndef __ShpLoader_hxx__
#define __ShpLoader_hxx__

#include <string>

#include <Point2D.hxx>

#ifdef GDAL_VERSION_2
#define GDAL_DATA_CLASS GDALDataset
#else
#define GDAL_DATA_CLASS OGRDataSource
#endif
class GDAL_DATA_CLASS;
class OGRLayer;

namespace Engine
{

class ShpLoader
{
private:
    
    GDAL_DATA_CLASS * _gdalData; //! Shapefile source
    OGRLayer * _activeLayer; //! Layer to be loaded

    /**
     * @brief Returns the index of field which name is fieldName inside _activeLayer.
     * 
     * @param fieldName Name of the checked field.
     * @return int 
     */
    int getFieldIndex( const std::string & fieldName );
public:
    /**
     * @brief Construct a new ShpLoader object.
     * 
     */
    ShpLoader( );

    /**
     * @brief Destroy the Loader object.
     * 
     */
    virtual ~ShpLoader( );

    /**
     * @brief this method opens the shapefile in file and allocates its info to _gdalData, selecting the first layer. 
     * Throws an exception if the file can't be opened or there are no layers
     * 
     * @param file Route to the shapefile.
     */
    void open( const std::string & file );

    /**
     * @brief Returns the number of layers ( 0 if no active _gdalData).
     * 
     * @return int 
     */
    int getNumLayers( );

    /**
     * @brief Sets the layer to be read. thows an exception if no active _gdalData.
     * 
     * @param activeLayerIndex Index of the layer on the shapefile.
     */
    void setActiveLayer( int activeLayerIndex );
    
    /**
     * @brief Returns the number of features in layer _activeLayer from source _gdalData. Throws an exception of layer or datasource were not loaded.
     * 
     * @return int 
     */
    int getNumFeatures( );

    // 
    /**
     * @brief Greturns the geometry of feature in position indexFeature. Throws an exception if index out of range.
     * 
     * @param indexFeature Index of the layer on the shapefile.
     * @return Point2D<int> 
     */
    Point2D<int> getPosition( int indexFeature );

    /**
     * @brief Returns the field fieldName of of feature in position indexFeature as string. Throws an exception if index out of range or fieldName does not exist.
     * 
     * @param indexFeature Index of the layer on the shapefile.
     * @param fieldName Name of the field.
     * @return std::string 
     */
    std::string getFieldAsString( int indexFeature, const std::string & fieldName );
    
    /**
     * @brief Returns the field fieldName of feature in position indexFeature as integer. Throws an exception if index out of range or fieldName does not exist.
     * 
     * @param indexFeature Index of the layer on the shapefile.
     * @param fieldName Name of the field.
     * @return int 
     */
    int getFieldAsInt( int indexFeature, const std::string & fieldName );
    
    /**
     * @brief Returns the field fieldName of feature in position indexFeature as double. Throws an exception if index out of range or fieldName does not exist.
     * 
     * @param indexFeature Index of the layer on the shapefile.
     * @param fieldName Name of the field.
     * @return double 
     */
    double getFieldAsFloat( int indexFeature, const std::string & fieldName );
};

} // namespace Engine

#endif // __ShpLoader_hxx__

