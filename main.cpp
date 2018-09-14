#include <iostream>
#include "Functions.h"

int main() {
    // Reader configuration object to store reader provider and reader unit selection.
    lla_ptr(ReaderConfiguration) readerConfig(new logicalaccess::ReaderConfiguration());

    // Set PCSC ReaderProvider by calling the Library Manager which will load the function from the corresponding plug-in
    lla_ptr(ReaderProvider) provider = logicalaccess::LibraryManager::getInstance()->getReaderProvider("PCSC");
    if (!provider) {
        std::cerr << "Could not get reader provider PCSC\nIs the config in the working directory?";
        return EXIT_FAILURE;
    }
    readerConfig->setReaderProvider(provider);

    // Create the default reader unit. On PC/SC, we will listen on all readers.
    readerConfig->setReaderUnit(readerConfig->getReaderProvider()->createReaderUnit());
    if (readerConfig->getReaderProvider()->getRPType() == "PCSC" &&
        readerConfig->getReaderProvider()->getReaderList().empty()) {
        std::cerr << "No readers on this system." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << readerConfig->getReaderProvider()->getReaderList().size() << " readers on this system." << std::endl;

    readerConfig->getReaderUnit()->connectToReader();

    // READING/WRITING STUFF HERE

    std::cout << "16 byte r/w test...\nWant me to read or (w)rite?" << std::endl;
    std::string s;
    std::cin >> s;
    bool write = s == "w";
    std::string strToWrite;

    if (write) {
        std::cout << "What you want to write? (Up to 15 bytes.)" << std::endl;
        std::cin >> strToWrite;

        if (strToWrite.size() > 15) {
            strToWrite = strToWrite.substr(0, 15);
            std::cout << "I will need to truncate this string.";
        }

        std::cout << "Ok, I'll write " << strToWrite << std::endl;
    }

    std::cout << "Please apply card..." << std::endl;
    if (readerConfig->getReaderUnit()->waitInsertion(15000)) {
        if (readerConfig->getReaderUnit()->connect()) {
            // Display few chip information

            std::cout << "Card detected on reader \"" << readerConfig->getReaderUnit()->getConnectedName() << "\"."
                      << std::endl;

            lla_ptr(Chip) chip = readerConfig->getReaderUnit()->getSingleChip();
            std::cout << "Card type: " << chip->getCardType() << std::endl;

            std::vector<unsigned char> csn = readerConfig->getReaderUnit()->getNumber(chip);
            std::cout << "Card unique manufacturer number: " << logicalaccess::BufferHelper::getHex(csn) << std::endl;

            // GET MIFARE-SPECIFIC POINTER

            lla_ptr(MifareChip) mifareChip = lla_ptr_dynamic_cast(MifareChip, chip);
            if (!mifareChip) {
                std::cerr << "I was expecting a Mifare chip. Goodbye" << std::endl;
                return EXIT_FAILURE;
            }

            // DO CHIP COMMANDS

            auto storageService = lla_ptr_dynamic_cast(StorageCardService, chip->getService(logicalaccess::CST_STORAGE));

            // We want to write data on block 1 of sector 3.
            lla_ptr(MifareLocation) memoryLocation(new logicalaccess::MifareLocation());
            memoryLocation->sector = 3;
            memoryLocation->block = 1;

            // Key to use for sector authentication
            lla_ptr(MifareAccessInfo) maiToUse(new logicalaccess::MifareAccessInfo());
            maiToUse->keyA->fromString("ff ff ff ff ff ff");      // Default key

            std::cout << "Poking 0x" << locationHex(memoryLocation) << std::endl;

            if (write) {
                writeStringToLocation(storageService, memoryLocation, maiToUse, nullptr, strToWrite, false);
                std::cout << "Successfully wrote " << strToWrite << " to the card." << std::endl;
            } else {
                std::cout << "Read: " << readStringFromLocation(storageService, memoryLocation, maiToUse, 16u, false) << std::endl;
            }

            // DONE WITH COMMANDS

            readerConfig->getReaderUnit()->disconnect();
        }

        std::cout << "Please remove card..." << std::endl;
        if (!readerConfig->getReaderUnit()->waitRemoval(15000)) {
            std::cerr << "Timeout - card not removed after 15 seconds. Ignoring card" << std::endl;
        }

        std::cout << "Card removed. Goodbye" << std::endl;
    } else {
        std::cout << "Timeout - no card inserted after 15 seconds. Goodbye" << std::endl;
    }

    // DONE READING

    readerConfig->getReaderUnit()->disconnectFromReader();

    return 0;
}