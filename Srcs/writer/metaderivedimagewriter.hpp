/* Copyright (c) 2015, Nokia Technologies Ltd.
 * All rights reserved.
 *
 * Licensed under the Nokia High-Efficiency Image File Format (HEIF) License (the "License").
 *
 * You may not use the High-Efficiency Image File Format except in compliance with the License.
 * The License accompanies the software and can be found in the file "LICENSE.TXT".
 *
 * You may also obtain the License at:
 * https://nokiatech.github.io/heif/license.txt
 */

#ifndef METADERIVEDIMAGEWRITER_HPP
#define METADERIVEDIMAGEWRITER_HPP

#include "context.hpp"
#include "metawriter.hpp"
#include "isomediafile.hpp"
#include <map>
#include <memory>
#include <string>

class DerivedItemMediaWriter;
class ItemPropertiesBox;
class ItemReferenceBox;
class MediaDataBox;
class MetaBox;

/**
 * @brief MetaBox writer for derived images
 * @details Write 'iden' (identity transformation), 'grid' (ImageGrid) and 'iovl' (ImageOverlay) type derived images,
 * and pre-derived images.
 * Properties related to 'iden' derived images are created too.
 */
class MetaDerivedImageWriter : public MetaWriter
{
public:
    /**
     * @brief Construct MetaDerivedImageWriter
     * @param config      Writer configuration
     * @param mediaWriter MediaWriter for this MetaDerivedImageWriter
     */
    MetaDerivedImageWriter(const IsoMediaFile::Derived& config, DerivedItemMediaWriter* mediaWriter);
    virtual ~MetaDerivedImageWriter() = default;

    /**
     * @brief Add data to a MetaBox based on configuration data given during writer construction.
     * @see MetaWriter::write()
     */
    void write(MetaBox* metaBox);

private:
    typedef std::uint32_t ItemId;
    typedef std::uint32_t UniqBsid;
    typedef std::vector<ItemId> ItemIdVector;
    typedef std::map<UniqBsid, ItemIdVector> ReferenceToItemIdMap;

    struct DerivationInfo
    {
        UniqBsid uniqBsid;                    ///< uniq_bsid from input configuration
        IsoMediaFile::ReferenceList refsList; ///< reference context uniq_bsids from input configuration, used to acquire item ID of the reference
        IsoMediaFile::IndexList indexList;    ///< 1-based image index, used to acquire item ID of the reference
        ItemIdVector itemIds;                 ///< item IDs generated by the writer
        ItemIdVector referenceItemIds;        ///< referenced item IDs
        std::string type;                     ///< 'grid', 'iovl', or 'iden' for identity transformation derivations
    };
    typedef std::map<UniqBsid, DerivationInfo> DerivationMap;

    struct ItemLocation
    {
        unsigned int offset;
        unsigned int length;
        unsigned int itemId;
    };
    typedef std::vector<ItemLocation> ItemLocations;

    DerivationMap mDerivations; ///< Class internal structure to hold information about derived images to be written
    DerivedItemMediaWriter* mDerivedItemMediaWriter; ///< Pointer to mdat writer of this meta writer
    IsoMediaFile::Derived mConfig; ///< Input configuration of the writer
    ReferenceToItemIdMap mReferenceToItemIdMap; ///< Mapping from UniqBsid and index to Item Id

    /**
     * @brief Add properties to Item Properties Box. This includes 'ispe' properties and properties used with identity
     * transformations (item type 'iden').
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     */
    void iprpWrite(MetaBox* metaBox);

    /**
     * @brief Add entries to the Item Information Box
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     */
    void iinfWrite(MetaBox* metaBox);

    /**
     * @brief Add entries to the Item Reference Box
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     */
    void irefWrite(MetaBox* metaBox);

    /**
     * @brief Add entries to iloc
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     * @param locations Location and size information about 'grid' and 'iovl' items to be created.
     */
    void ilocWrite(MetaBox* metaBox, const ItemLocations& locations);

    /**
     * Insert pre-derived image 'base' references to Item Reference Box
     * @param [in,out]     metaBox Pointer to the MetaBox where to add data.
     * @param preRefsList  Vector of uniq_bsid values which define contexts of pre-derived images
     * @param preIdxsList  For each entry in preRefsList, here is a vector of image indexes pointing to pre-deried images (1-based index)
     * @param baseRefsList Vector of uniq_bsid values which define contexts of base references of pre-derived images
     * @param baseIdxsList For each entry in baseRefsList, here is a vector of image indexes pointing to base images (1-based index)
     */
    void insertBaseReferences(MetaBox* metaBox, const IsoMediaFile::ReferenceList& preRefsList,
        const IsoMediaFile::IndexList& preIdxsList, const IsoMediaFile::ReferenceList& baseRefsList,
        const IsoMediaFile::IndexList& baseIdxsList) const;

    /**
     * Insert derived image 'dimg' references to Item Reference Box
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     * @param derivation DerivationInfo for which write the 'dimg' reference
     */
    void insertDimgReferences(MetaBox* metaBox, const DerivationInfo& derivation);

    /**
     * Create internal data structure from 'iden' identity derivation type derived images in content configuration.
     * Item IDs are assigned here.
     * @return Entries for each 'irot', 'rloc' and 'clap' in input configuration.
     */
    DerivationMap processIdenDerivations() const;

    /**
     * Create internal data structure from 'grid' and 'iovl' type derived images in content configuration
     * Item IDs are assigned here.
     * @param [in,out] mdat               MediaDataBox including item data for each 'grid' and 'iovl' item
     * @param [out]    itemLocationVector 'grid' and 'iovl' item data locations, sizes and IDs.
     * @return Entries for each 'grid' and 'iovl' in input configuration.
     */
    DerivationMap processOtherDerivations(MediaDataBox& mdat, ItemLocations& itemLocationVector) const;

    /**
     * @return Mapping from uniq_bsid context references and image indexes to item IDs
     * @todo Need for this method could be refactored away, if there was a global service for such mappings.
     */
    ReferenceToItemIdMap createReferenceToItemIdMap() const;

    /**
     * @brief Add associations to existing 'ispe' item properties.
     * @details Length of fromItems and toItems vectors must be same. Mapping is done to 'ispe' of
     * fromItems[0] to toItems[0], fromItems[1] to toItems[1], etc.
     * @param [in,out] metaBox Pointer to the MetaBox where to add data.
     * @param fromItems        Item IDs with 'ispe' properties
     * @param toItems          Item IDs to which 'ispe' mapping will be done
     */
    void linkIspeProperties(MetaBox* metaBox, const ItemIdVector& fromItems, const ItemIdVector& toItems) const;

    /**
     * Add reference item IDs to internal data structure.
     * @param [in,out] derivations Derivations to be processed.
     */
    void addItemIdReferences(MetaDerivedImageWriter::DerivationMap& derivations) const;
};

#endif /* end of include guard: METADERIVEDIMAGEWRITER_HPP */

