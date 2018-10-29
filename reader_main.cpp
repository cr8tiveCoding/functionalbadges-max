#define READER
#include <iostream>
#include "functions.h"
#include "data.h"

#define OUT_PATH "./read_output"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
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

    for (;;) {
        std::cout << "Please apply card..." << std::endl;
        if (readerConfig->getReaderUnit()->waitInsertion(0)) {
            if (readerConfig->getReaderUnit()->connect()) {

                try {
                    lla_ptr(Chip) chip = readerConfig->getReaderUnit()->getSingleChip();

                    lla_ptr(MifareChip) mifareChip = lla_ptr_dynamic_cast(MifareChip, chip);
                    if (!mifareChip) {
                        std::cerr << "Card must be Mifare 1K. Please remove card" << std::endl;
                        readerConfig->getReaderUnit()->waitRemoval(0);
                        continue;
                    }

                    // DO CHIP COMMANDS

                    auto storageService = lla_ptr_dynamic_cast(StorageCardService,
                                                               chip->getService(logicalaccess::CST_STORAGE));
                    auto accessInfo = lla_ptr(MifareAccessInfo)(new logicalaccess::MifareAccessInfo());
                    accessInfo->keyA->fromString("ff ff ff ff ff ff");

                    CardData cd;
                    cd.storageService = storageService;
                    cd.accessInfo = accessInfo;

                    auto name = readStudentName(cd);
                    auto id = readStudentId(cd);
                    auto grade = readStudentGrade(cd);
                    auto uuid = readUuid(cd);

                    std::cout << appendToFile(OUT_PATH, "Student Name: " + std::get<0>(name) + " " + std::get<1>(name) + "\n");
                    std::cout << appendToFile(OUT_PATH, "Student Id: " + id + "\n");
                    std::cout << appendToFile(OUT_PATH, "Student Grade: " + std::to_string(grade) + "\n");
                    std::cout << appendToFile(OUT_PATH, "UUID: " + boost::uuids::to_string(uuid) + "\n");

                    std::cout << appendToFile(OUT_PATH, "\n");

                    readerConfig->getReaderUnit()->waitRemoval(0);

                } catch (logicalaccess::CardException& e) {
                    std::cerr << "Caught an exception: " << e.error_code() << " -- " << e.what() << std::endl <<
                        "Did you remove the card too soon?" << std::endl;
                    readerConfig->getReaderUnit()->waitRemoval(15);
                }
            } else {
                std::cerr << "Card connection failure" << std::endl;
            }
        } else {
            std::cout << "Timeout" << std::endl;
        }
    }
}
#pragma clang diagnostic pop