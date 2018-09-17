#ifndef FUNCTIONALBADGES_FUNCTIONS_H
#define FUNCTIONALBADGES_FUNCTIONS_H

#include <vector>
#include <cstdint>

#include <boost/format.hpp>

#include "macro.h"

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
inline std::vector<uint8_t> readFromLocation(lla_ptr(StorageCardService) storageService,
                                             lla_ptr(MifareLocation) memoryLocation,
                                             lla_ptr(MifareAccessInfo) accessInfo,
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
inline std::string readStringFromLocation(lla_ptr(StorageCardService) storageService,
                                          lla_ptr(MifareLocation) memoryLocation,
                                          lla_ptr(MifareAccessInfo) accessInfo,
                                          unsigned int bytesToRead,
                                          bool autoSwitchBlock) {
    auto data = readFromLocation(storageService, memoryLocation, accessInfo, bytesToRead, autoSwitchBlock);
    return std::string(data.begin(), data.end());
}

#ifdef WRITER

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
inline void writeDataToLocation(lla_ptr(StorageCardService) storageService,
                                lla_ptr(MifareLocation) memoryLocation,
                                lla_ptr(MifareAccessInfo) accessInfo,
                                lla_ptr(MifareAccessInfo) accessInfoToWrite,
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
inline void writeStringToLocation(lla_ptr(StorageCardService) storageService,
                                  lla_ptr(MifareLocation) memoryLocation,
                                  lla_ptr(MifareAccessInfo) accessInfo,
                                  lla_ptr(MifareAccessInfo) accessInfoToWrite,
                                  std::string data,
                                  bool autoSwitchBlock) {
    writeDataToLocation(storageService, memoryLocation, accessInfo, accessInfoToWrite,
                        std::vector<uint8_t>(data.begin(), data.end()), autoSwitchBlock);
}

#endif //WRITER

inline std::string locationHex(int sector, int block, int byte) {
    int address = (sector * (16 * 4)) + (block * 16) + byte;

    if (address > MAX_ADDRESS)
        throw std::out_of_range("max address of mifare 1K is 0x3ff");

    char buf[4];
    sprintf(buf, "%x", address);
    return std::string(buf);
}

inline std::string locationHex(lla_ptr(MifareLocation) mem) {
    return locationHex(mem->sector, mem->block, mem->byte);
}

inline std::string describeLocation(lla_ptr(MifareLocation) mem) {
    return boost::str(boost::format("0x%4%\nSector %1%\nBlock %2%\nByte %3%") % mem->sector % mem->block % mem->byte %
                     locationHex(mem));
}

inline lla_ptr(MifareLocation) location(const int* a) {
    lla_ptr(MifareLocation) l(new logicalaccess::MifareLocation());
    l->sector = a[0];
    l->block = a[1];
    return l;
}

#endif //FUNCTIONALBADGES_FUNCTIONS_H
