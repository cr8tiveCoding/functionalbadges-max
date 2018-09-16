#ifndef FUNCTIONALBADGES_MACRO_H
#define FUNCTIONALBADGES_MACRO_H

#include <boost/shared_ptr.hpp>

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

#endif //FUNCTIONALBADGES_MACRO_H
