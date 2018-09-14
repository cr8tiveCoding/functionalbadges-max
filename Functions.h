//
// Created by max on 9/14/18.
//

#ifndef FUNCTIONALBADGES_FUNCTIONS_H
#define FUNCTIONALBADGES_FUNCTIONS_H

#include <vector>
#include <cstdint>

#include <boost/format.hpp>

#include <logicalaccess/dynlibrary/librarymanager.hpp>
#include <logicalaccess/readerproviders/readerconfiguration.hpp>
#include <logicalaccess/cards/chip.hpp>
#include <logicalaccess/plugins/readers/pcsc/pcscreaderprovider.hpp>
#include <logicalaccess/bufferhelper.hpp>
#include <logicalaccess/services/storage/storagecardservice.hpp>
#include <logicalaccess/plugins/cards/mifare/mifarechip.hpp>
#include <logicalaccess/plugins/cards/mifare/mifarelocation.hpp>
#include <logicalaccess/plugins/cards/mifare/mifareaccessinfo.hpp>


template <typename T>
using shared_ptr = std::shared_ptr<T>;

#define lla_ptr(T) std::shared_ptr<logicalaccess::T>
#define lla_ptr_dynamic_cast(T, R) std::dynamic_pointer_cast<logicalaccess::T>(R)

constexpr int MAX_ADDRESS = 1023;

/**
 * Read a specified amount of bytes starting at the given location.
 *
 * @param storageService The Mifare storage service
 * @param memoryLocation The location to start reading from
 * @param accessInfo The key used to authenticate read permissions
 * @param bytesToRead The number of bytes to read
 * @param autoSwitchBlock If the block and sector indices will be automatically managed
 * @return A vector containing the read bytes
 */
std::vector<uint8_t> readFromLocation(lla_ptr(StorageCardService) const &storageService,
                                      lla_ptr(MifareLocation) const &memoryLocation,
                                      lla_ptr(MifareAccessInfo) const &accessInfo,
                                      unsigned int bytesToRead,
                                      bool autoSwitchBlock) {
    return storageService->readData(memoryLocation, accessInfo, bytesToRead,
                                    autoSwitchBlock ? logicalaccess::CB_AUTOSWITCHAREA
                                                    : logicalaccess::CB_DEFAULT);
}

/**
 * Read a string of the amount of bytes starting at the given location.
 *
 * @param storageService The Mifare storage service
 * @param memoryLocation The location to start reading from
 * @param accessInfo The key used to authenticate read permissions
 * @param bytesToRead The number of bytes to read
 * @param autoSwitchBlock If the block and sector indices will be automatically managed
 * @return The read string
 */
std::string readStringFromLocation(lla_ptr(StorageCardService) const &storageService,
                                   lla_ptr(MifareLocation) const &memoryLocation,
                                   lla_ptr(MifareAccessInfo) const &accessInfo,
                                   unsigned int bytesToRead,
                                   bool autoSwitchBlock) {
    auto data = readFromLocation(storageService, memoryLocation, accessInfo, bytesToRead, autoSwitchBlock);
    return std::string(data.begin(), data.end());
}

/**
 * Write the specified data starting at the given location
 *
 * @param storageService The Mifare storage service
 * @param memoryLocation The location to start writing from
 * @param accessInfo The key used to authenticate read and write permissions
 * @param accessInfoToWrite If not null, the keys to overwrite the existing keys
 * @param data The data to write
 * @param autoSwitchBlock If the block and sector indices will be automatically managed
 */
void writeDataToLocation(lla_ptr(StorageCardService) const &storageService,
                         lla_ptr(MifareLocation) const &memoryLocation,
                         lla_ptr(MifareAccessInfo) const &accessInfo,
                         lla_ptr(MifareAccessInfo) const &accessInfoToWrite,
                         const std::vector<unsigned char> &data,
                         bool autoSwitchBlock) {
    storageService->writeData(memoryLocation, accessInfo, accessInfoToWrite, data,
                              autoSwitchBlock ? logicalaccess::CB_AUTOSWITCHAREA
                                              : logicalaccess::CB_DEFAULT);
}

/**
 * Write the specified string starting at the given location
 *
 * @param storageService The Mifare storage service
 * @param memoryLocation The location to start writing from
 * @param accessInfo The key used to authenticate read and write permissions
 * @param accessInfoToWrite If not null, the keys to overwrite the existing keys
 * @param data The string to write
 * @param autoSwitchBlock If the block and sector indices will be automatically managed
 */
void writeStringToLocation(lla_ptr(StorageCardService) const &storageService,
                           lla_ptr(MifareLocation) const &memoryLocation,
                           lla_ptr(MifareAccessInfo) const &accessInfo,
                           lla_ptr(MifareAccessInfo) const &accessInfoToWrite,
                           std::string const &data,
                           bool autoSwitchBlock) {
    writeDataToLocation(storageService, memoryLocation, accessInfo, accessInfoToWrite,
                        std::vector<uint8_t>(data.begin(), data.end()), autoSwitchBlock);
}

std::string locationHex(int sector, int block, int byte) {
    int address = (sector * (16 * 4)) + (block * 16) + byte;

    if (address > MAX_ADDRESS)
        throw std::out_of_range("max address of mifare 1K is 0x3ff");

    char buf[4];
    sprintf(buf, "%x", address);
    return std::string(buf);
}

std::string locationHex(lla_ptr(MifareLocation) const &mem) {
    return locationHex(mem->sector, mem->block, mem->byte);
}

std::string describeLocation(lla_ptr(MifareLocation) const &mem) {
    return boost::str(boost::format("0x%4%\nSector %1%\nBlock %2%\nByte %3%") % mem->sector % mem->block % mem->byte %
                     locationHex(mem));
}

#endif //FUNCTIONALBADGES_FUNCTIONS_H
