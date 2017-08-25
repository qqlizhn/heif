#include "hevcimagefilereader.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if ( argc != 3 ) {
        cout << "usage: heiftojpeg <input_file_name> <output_file_name>";
        return 1;
    }

    char *input_file_name = argv[1];
    char *output_file_name = argv[2];

    cout << "converting " << input_file_name << " to " << output_file_name << "\n";

    HevcImageFileReader reader;
    ImageFileReaderInterface::DataVector data;
    ImageFileReaderInterface::IdVector itemIds;

    reader.initialize(input_file_name);
    const auto& properties = reader.getFileProperties();

    // Verify that the file has one or several images in the MetaBox
    if (not (properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasSingleImage) ||
        properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasImageCollection)))
    {
        cout << "Input has no image\n";
        return 1;
    }


    // Find the item ID of the first master image
    const uint32_t contextId = properties.rootLevelMetaBoxProperties.contextId;
    cout << "A\n";
    reader.getItemListByType(contextId, "master", itemIds);
    cout << "B\n";
    const uint32_t masterId = itemIds.at(0);
    cout << "C\n";

    const auto& metaBoxFeatures = properties.rootLevelMetaBoxProperties.metaBoxFeature; // For convenience
    cout << "D\n";
    if (metaBoxFeatures.hasFeature(ImageFileReaderInterface::MetaBoxFeature::HasThumbnails))
    {
        cout << "E\n";
        // Thumbnail references ('thmb') are from the thumbnail image to the master image
        reader.getReferencedToItemListByType(contextId, masterId, "thmb", itemIds);
        cout << "F\n";
        const uint32_t thumbnailId = itemIds.at(0);
        cout << "G\n";

        reader.getItemDataWithDecoderParameters(contextId, thumbnailId, data);
        cout << "H\n";
        // ...decode data and display the image, show master image later
    }
    else
    {
        cout << "I\n";
        // There was no thumbnail, show just the master image
        reader.getItemDataWithDecoderParameters(contextId, masterId, data);
        cout << "J\n";
        // ...decode and display...
    }

    cout << "FIN\n";

    return 0;
}

