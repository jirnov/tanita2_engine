/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RESOURCE_CONVERTER_CLASSES_H_
#define _RESOURCE_CONVERTER_CLASSES_H_


namespace resourceManager
{

// Base class for file converters
class ResourceConverterBase
{
public:
    // Returns true if successfully converts file to native format
    virtual bool convert( const ResourceId & path ) = 0;
};

// Converter from PNG to DDS
class DDSConverter : public ResourceConverterBase
{
public:
    // Returns true if successfully converts file to native format
    virtual bool convert( const ResourceId & path );
};

}; // End of resourceManager namespace


#endif // _RESOURCE_CONVERTER_CLASSES_H_
