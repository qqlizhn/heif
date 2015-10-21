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

#ifndef ITEMLOCATIONBOX_HPP
#define ITEMLOCATIONBOX_HPP

#include "fullbox.hpp"
#include "bitstream.hpp"

#include <iterator>
#include <vector>

/** @brief Item Location Extent data structure as defined in the ISOBMFF specification. */
struct ItemLocationExtent
{
    std::uint64_t mExtentIndex = 0;
    std::uint64_t mExtentOffset = 0;
    std::uint64_t mExtentLength = 0;
};

typedef std::vector<ItemLocationExtent> ExtentList; /// vector of item location extents

/** @brief Item Location class.
 *  @details Item locations can be provided by an offset from the file, idat or item itself.
 *  @details The construction method defines the offset mechanism. **/
class ItemLocation
{
public:
    ItemLocation();
    ~ItemLocation() = default;

    /// Construction Method
    enum class ConstructionMethod
    {
        FILE_OFFSET = 0,
        IDAT_OFFSET = 1,
        ITEM_OFFSET = 2
    };

    /** @brief Set the Item ID
     *  @param [in] itemID Item ID */
    void setItemID(std::uint16_t itemID);

    /** @brief Get the Item ID
     *  @return Item ID */
    std::uint16_t getItemID() const;

    /** @brief Set the construction method
     *  @param [in] constructionMethod Enumerated Construction Method */
    void setConstructionMethod(ConstructionMethod constructionMethod);

    /** @brief Get the construction method
     *  @return Enumerated Construction Method */
    ConstructionMethod getConstructionMethod() const;

    /** @brief Set the data reference index
     *  @param [in] dataReferenceIndex Data reference index value */
    void setDataReferenceIndex(std::uint16_t dataReferenceIndex);

    /** @brief Get the data reference index
     *  @return Data reference index */
    std::uint16_t getDataReferenceIndex() const;

    /** @brief Set the base offset value
     *  @param [in] baseOffset Base offset value */
    void setBaseOffset(std::uint64_t baseOffset);

    /** @brief Get the base offset value
     *  @return Base Offset */
    std::uint64_t getBaseOffset() const;

    /** @brief Get the number of extents
     *  @return Extent Count */
    std::uint16_t getExtentCount() const;

    /** @brief Add an item location extent
     *  @param [in] extent Item Location Extent data structure */
    void addExtent(const ItemLocationExtent& extent);

    /** @brief Get the list of defined extents
     *  @return Extent List */
    const ExtentList& getExtentList() const;

    /** @brief Get an extent which is present in the etent list
     *  @param [in] i 0-based extent index
     *  @return Item Location Extent data strcuture */
    const ItemLocationExtent& getExtent(unsigned int i) const;

    /** @brief Writes the ItemLocation to the bitstream in the standard-defined format
     *  @param [in] version version of the ItemLocation
     *  @param [out] bitstr output BitStream */
    void write(BitStream& bitstr, unsigned int version);

private:
    std::uint16_t mItemID;  /// item ID
    ConstructionMethod mConstructionMethod; /// construction method enumeration
    std::uint16_t mDataReferenceIndex; /// data reference index
    std::uint64_t mBaseOffset; /// base offset value
    ExtentList mExtentList; /// list of extents
};

typedef std::vector<ItemLocation> ItemLocationVector; /// vector of Item Locations


/** @brief Item Location Box class. Extends from FullBox.
 *  @details 'iloc' box contains information about an item's location and its extents **/

class ItemLocationBox : public FullBox
{
public:
    ItemLocationBox();
    virtual ~ItemLocationBox() = default;

    /** @brief Set the offset size
     *  @param [in] offsetSize Offset size {0,4 or 8} */
    void setOffsetSize(std::uint8_t offsetSize);

    /** @brief Get the offset size
     *  @return Offset size {0,4 or 8} */
    std::uint8_t getOffsetSize() const;

    /** @brief Set the length size
     *  @param [in] lengthSize Length size {0,4 or 8} */
    void setLengthSize(std::uint8_t lengthSize);

    /** @brief Get the length size
     *  @return Length size {0,4 or 8} */
    std::uint8_t getLengthSize() const;

    /** @brief Set the base offset size
     *  @param [in] baseOffsetSize Base offset size {0,4 or 8} */
    void setBaseOffsetSize(std::uint8_t baseOffsetSize);

    /** @brief Get the base offset size
     *  @return Base Offset size {0,4 or 8} */
    std::uint8_t getBaseOffsetSize() const;

    /** @brief Set the index size
     *  @param [in] indexSize Index size {0,4 or 8} */
    void setIndexSize(std::uint8_t indexSize);

    /** @brief Get the index size
     *  @return Index size {0,4 or 8} */
    std::uint8_t getIndexSize() const;

    /** @brief Set the number of items
     *  @param [in] itemCount number of items to be listed. */
    void setItemCount(std::uint16_t itemCount);

    /** @brief Get the number of items
     *  @return Index size {0,4 or 8} */
    std::uint16_t getItemCount() const;

    /** @brief Add an item location entry
     *  @param [in] itemLoc ItemLocation data structure */
    void addLocation(const ItemLocation& itemLoc);

    /** @brief Add an Extent
     *  @param [in] itemId Item ID to use
     *  @param [in] extent ItemLocationExtent data structure */
    void addExtent(std::uint16_t itemId, const ItemLocationExtent& extent);

    /** @brief Check if an itemId alreadyhas an item location entry
     *  @param [in] itemId Item ID
     *  @return TRUE if present, FALSE if not present */
    bool hasItemIdEntry(std::uint16_t itemId) const;

    /** @brief Set the item data reference index
     *  @param [in] itemId Item ID
     *  @param [in] dataReferenceIndex Data Reference Index
     *  @return TRUE if item with item ID is found and data refrence is set, FALSE if item with item Id not found */
    bool setItemDataReferenceIndex(std::uint16_t itemId, std::uint16_t dataReferenceIndex);

    /** @brief Get the item location vector
     *  @return Item Location vector of Item Location entries */
    ItemLocationVector& getItemLocations();

    /** @brief Creates the bitstream that represents the box in the ISOBMFF file
     *  @param [out] bitstr Bitstream that contains the box data. */
    void writeBox(BitStream& bitstr);

    /** @brief Parses an ItemLocationBox bitstream and fills in the necessary member variables
     *  @param [in]  bitstr Bitstream that contains the box data */
    void parseBox(BitStream& bitstr);

    /** @brief Get Item Location entry of the item with given ID
     *  @param [in] itemID Item ID to be searched.
     *  @return ItemLocation entry of the given Item ID
     *  @throws Run-time Exception if an entry with the given item Id is not found.*/
    const ItemLocation& getItemLocationForID(unsigned int itemID) const;

private:
    std::uint8_t mOffsetSize; /// offset size {0,4, or 8}
    std::uint8_t mLengthSize; /// length size {0,4, or 8}
    std::uint8_t mBaseOffsetSize; /// base offset size {0,4, or 8}
    std::uint8_t mIndexSize; /// index size {0,4, or 8} and only if version == 1, otherwise reserved
    ItemLocationVector mItemLocations;  /// vector of item location entries

    ItemLocationVector::const_iterator findItem(std::uint16_t itemId) const; /// find an item with given itemId and return as a const
    ItemLocationVector::iterator findItem(std::uint16_t itemId); /// find an item with given itemId and return
};

#endif /* end of include guard: ITEMLOCATIONBOX_HPP */
