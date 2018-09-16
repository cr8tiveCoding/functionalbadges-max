#ifndef FUNCTIONALBADGES_DATA_H
#define FUNCTIONALBADGES_DATA_H

#include "macro.h"
#include "functions.h"

constexpr int firstNameLocation [2] = {3, 0},
              lastNameLocation  [2] = {3, 1},
              idLocation        [2] = {3, 2},
              gradeLocation     [2] = {4, 0};

typedef struct {
    lla_ptr(StorageCardService) storageService;
    lla_ptr(MifareAccessInfo) accessInfo;
} CardData;

inline void writeStudentName(CardData const& cd, std::string first, std::string last) {
    writeStringToLocation(cd.storageService, location(firstNameLocation), cd.accessInfo, nullptr, first, false);
    writeStringToLocation(cd.storageService, location(lastNameLocation), cd.accessInfo, nullptr, last, false);
}

inline std::tuple<std::string, std::string> readStudentName(CardData const& cd) {
    return std::make_tuple(readStringFromLocation(cd.storageService, location(firstNameLocation), cd.accessInfo, 16, false),
                           readStringFromLocation(cd.storageService, location(lastNameLocation), cd.accessInfo, 16, false));
}

#endif //FUNCTIONALBADGES_DATA_H
