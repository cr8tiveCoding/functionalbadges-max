#ifndef FUNCTIONALBADGES_DATA_H
#define FUNCTIONALBADGES_DATA_H

#include "macro.h"
#include "functions.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

constexpr int firstNameLocation [2] = {3, 0},
              lastNameLocation  [2] = {3, 1},
              idLocation        [2] = {3, 2},
              gradeLocation     [2] = {4, 0},
              uuidLocation      [2] = {4, 1};

typedef struct {
    lla_ptr(StorageCardService) storageService;
    lla_ptr(MifareAccessInfo) accessInfo;
} CardData;

inline std::tuple<std::string, std::string> readStudentName(CardData const& cd) {
    return std::make_tuple(readStringFromLocation(cd.storageService, location(firstNameLocation), cd.accessInfo, 16, false),
                           readStringFromLocation(cd.storageService, location(lastNameLocation), cd.accessInfo, 16, false));
}


inline std::string readStudentId(CardData const& cd) {
    return readStringFromLocation(cd.storageService, location(idLocation), cd.accessInfo, 16, false);
}

inline uint8_t readStudentGrade(CardData const& cd) {
    return readFromLocation(cd.storageService, location(gradeLocation), cd.accessInfo, 1, false)[0];
}

inline boost::uuids::uuid readUuid(CardData const& cd) {
    std::vector<uint8_t> bytes = readFromLocation(cd.storageService, location(uuidLocation), cd.accessInfo, 16, false);
    boost::uuids::uuid u {};
    uint8_t* bytesArr = &bytes[0];
    memcpy(&u, bytesArr, 16);
    return u;
}

#ifdef WRITER

inline boost::uuids::uuid genUuid() {
    return boost::uuids::random_generator()();
}

inline std::vector<uint8_t> uuidBytes(boost::uuids::uuid u) {
    std::vector<uint8_t> v(u.size());
    std::copy(u.begin(), u.end(), v.begin());
    return v;
}

inline void writeStudentGrade(CardData const& cd, uint8_t grade) {
    writeDataToLocation(cd.storageService, location(gradeLocation), cd.accessInfo, nullptr, {grade}, false);
}

inline void writeStudentId(CardData const& cd, std::string id) {
    writeStringToLocation(cd.storageService, location(idLocation), cd.accessInfo, nullptr, id, false);
}

inline void writeStudentName(CardData const& cd, std::string first, std::string last) {

    if (first.length() > 15)
        first = first.substr(0, 14) + "~";

    if (last.length() > 15)
        last = last.substr(0, 14) + "~";


    writeStringToLocation(cd.storageService, location(firstNameLocation), cd.accessInfo, nullptr, first, false);
    writeStringToLocation(cd.storageService, location(lastNameLocation), cd.accessInfo, nullptr, last, false);
}

inline boost::uuids::uuid writeUuid(CardData const& cd) {
    boost::uuids::uuid uuid = genUuid();
    writeDataToLocation(cd.storageService, location(uuidLocation), cd.accessInfo, nullptr, uuidBytes(uuid), false);
    return uuid;
}

#endif //WRITER

#endif //FUNCTIONALBADGES_DATA_H
