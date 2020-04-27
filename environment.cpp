#include "environment.h"

Environment::Environment()
{

}

SystemInfo Environment::systemInfo =
    {
      QSysInfo::productType(),
      QSysInfo::productVersion(),
      QSysInfo::prettyProductName()
    };

SystemInfo Environment::getSystemInfo()
{
    return Environment::systemInfo;
}
