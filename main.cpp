#include <iostream>
#include <logicalaccess/dynlibrary/librarymanager.hpp>
#include <logicalaccess/readerproviders/readerconfiguration.hpp>
#include <logicalaccess/cards/chip.hpp>
#include <logicalaccess/plugins/readers/pcsc/pcscreaderprovider.hpp>
#include <logicalaccess/bufferhelper.hpp>
#include <logicalaccess/services/storage/storagecardservice.hpp>
#include <logicalaccess/plugins/cards/mifare/mifarechip.hpp>
#include <logicalaccess/plugins/cards/mifare/mifarelocation.hpp>
#include <logicalaccess/plugins/cards/mifare/mifareaccessinfo.hpp>

int main() {
    // Reader configuration object to store reader provider and reader unit selection.
    std::shared_ptr<logicalaccess::ReaderConfiguration> readerConfig(new logicalaccess::ReaderConfiguration());

    // Set PCSC ReaderProvider by calling the Library Manager which will load the function from the corresponding plug-in
    std::shared_ptr<logicalaccess::ReaderProvider> p = logicalaccess::LibraryManager::getInstance()->getReaderProvider(
            "PCSC");
    if (!p) {
        std::cerr << "Could not get reader provider PCSC\nIs the config in the working directory?";
        return EXIT_FAILURE;
    }
    readerConfig->setReaderProvider(p);

    // Create the default reader unit. On PC/SC, we will listen on all readers.
    readerConfig->setReaderUnit(readerConfig->getReaderProvider()->createReaderUnit());
    if (readerConfig->getReaderProvider()->getRPType() == "PCSC" &&
        readerConfig->getReaderProvider()->getReaderList().empty()) {
        std::cerr << "No readers on this system." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << readerConfig->getReaderProvider()->getReaderList().size() << " readers on this system." << std::endl;

    readerConfig->getReaderUnit()->connectToReader();

    // READING STUFF HERE

    std::cout << "Waiting 15 seconds for a card insertion..." << std::endl;
    if (readerConfig->getReaderUnit()->waitInsertion(15000)) {
        if (readerConfig->getReaderUnit()->connect()) {
            // Display few chip information

            std::cout << "Card inserted on reader \"" << readerConfig->getReaderUnit()->getConnectedName() << "\"."
                      << std::endl;

            std::shared_ptr<logicalaccess::Chip> chip = readerConfig->getReaderUnit()->getSingleChip();
            std::cout << "Card type: " << chip->getCardType() << std::endl;

            std::vector<unsigned char> csn = readerConfig->getReaderUnit()->getNumber(chip);
            std::cout << "Card unique manufacturer number : " << logicalaccess::BufferHelper::getHex(csn) << std::endl;

            // GET MIFARE-SPECIFIC POINTER

            std::shared_ptr<logicalaccess::MifareChip> mifareChip = std::dynamic_pointer_cast<logicalaccess::MifareChip>(chip);
            if (!mifareChip) {
                std::cerr << "I was expecting a Mifare chip." << std::endl;
                return EXIT_FAILURE;
            }

            // DO CHIP COMMANDS

            std::cout << "Want me to read or (w)rite?" << std::endl;
            std::string s;
            std::cin >> s;
            bool write = s == "w";

            std::shared_ptr<logicalaccess::StorageCardService> storage =
                    std::dynamic_pointer_cast<logicalaccess::StorageCardService>(chip->getService(logicalaccess::CST_STORAGE));

            std::shared_ptr<logicalaccess::Location> location;
            std::shared_ptr<logicalaccess::AccessInfo> aiToUse;
            std::shared_ptr<logicalaccess::AccessInfo> aiToWrite;

            // We want to write data on sector 3.
            std::shared_ptr<logicalaccess::MifareLocation> mlocation(new logicalaccess::MifareLocation());
            mlocation->sector = 3;
            mlocation->block = 1;
            location = mlocation;

            // Key to use for sector authentication
            std::shared_ptr<logicalaccess::MifareAccessInfo> maiToUse(new logicalaccess::MifareAccessInfo());
            maiToUse->keyA->fromString("ff ff ff ff ff ff");      // Default key
            aiToUse = maiToUse;

            if (write) {
                std::vector<uint8_t> toWrite;
                std::string in;
                std::cout << "What you want to write? truncated to 16 bytes" << std::endl;
                std::cin >> in;
                auto inc = in.c_str();
                for (int i = 0; i < 16; ++i) {
                    toWrite.push_back((unsigned char &&) inc[i]);
                }

                storage->writeData(location, aiToUse, nullptr, toWrite, logicalaccess::CB_DEFAULT);

                // read data back to confirm

                std::vector<uint8_t> readData;
                readData = storage->readData(location, aiToUse, 16, logicalaccess::CB_DEFAULT);
                std::string str(readData.begin(), readData.end());
                std::cout << "Successfully read " << str << std::endl;
            } else {
                std::vector<uint8_t> readData;
                readData = storage->readData(location, aiToUse, 16, logicalaccess::CB_DEFAULT);
                std::string str(readData.begin(), readData.end());
                std::cout << "Read: " << str << std::endl;
            }

            // DONE WITH COMMANDS

            readerConfig->getReaderUnit()->disconnect();
        }

        std::cout << "Logical automatic card removal in 15 seconds..." << std::endl;
        if (!readerConfig->getReaderUnit()->waitRemoval(15000)) {
            std::cerr << "Card removal forced." << std::endl;
        }

        std::cout << "Card removed." << std::endl;
    } else {
        std::cout << "No card inserted." << std::endl;
    }

    // DONE READING

    readerConfig->getReaderUnit()->disconnectFromReader();

    return 0;
}