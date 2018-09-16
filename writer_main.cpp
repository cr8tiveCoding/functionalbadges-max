#include <iostream>
#include "functions.h"
#include "data.h"

int main() {
    lla_ptr(ReaderConfiguration) readerConfig(new logicalaccess::ReaderConfiguration());

    lla_ptr(ReaderProvider) provider = logicalaccess::LibraryManager::getInstance()->getReaderProvider("PCSC");
    if (!provider) {
        std::cerr << "Could not get reader provider PCSC\nIs the config in the working directory?";
        return EXIT_FAILURE;
    }
    readerConfig->setReaderProvider(provider);

    readerConfig->setReaderUnit(readerConfig->getReaderProvider()->createReaderUnit());
    if (readerConfig->getReaderProvider()->getRPType() == "PCSC" &&
        readerConfig->getReaderProvider()->getReaderList().empty()) {
        std::cerr << "No readers on this system." << std::endl;
        return EXIT_FAILURE;
    }

    readerConfig->getReaderUnit()->connectToReader();

    std::string fName, lName;
    std::cout << "First name: ";
    std::cin >> fName;
    std::cout << "Last name: ";
    std::cin >> lName;

    std::cout << "Please apply card..." << std::endl;
    if (readerConfig->getReaderUnit()->waitInsertion(0)) {
        if (readerConfig->getReaderUnit()->connect()) {

            lla_ptr(Chip) chip = readerConfig->getReaderUnit()->getSingleChip();

            lla_ptr(MifareChip) mifareChip = lla_ptr_dynamic_cast(MifareChip, chip);
            if (!mifareChip) {
                std::cerr << "I was expecting a Mifare chip. Goodbye" << std::endl;
                return EXIT_FAILURE;
            }

            // DO CHIP COMMANDS

            auto storageService = lla_ptr_dynamic_cast(StorageCardService, chip->getService(logicalaccess::CST_STORAGE));
            auto accessInfo     = lla_ptr(MifareAccessInfo)(new logicalaccess::MifareAccessInfo());
            accessInfo->keyA->fromString("ff ff ff ff ff ff");

            CardData cd;
            cd.storageService = storageService;
            cd.accessInfo = accessInfo;

            writeStudentName(cd, fName, lName);
            std::cout << "Wrote student name " << fName << " " << lName << std::endl;

        } else {
            std::cerr << "Card connection failure" << std::endl;
        }
    } else {
        std::cout << "Timeout" << std::endl;
    }

    // DONE READING

    readerConfig->getReaderUnit()->disconnectFromReader();

    std::cout << "Done." << std::endl;

    return 0;
}