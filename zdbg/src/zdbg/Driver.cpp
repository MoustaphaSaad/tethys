#include "zdbg/Driver.h"

namespace zdbg
{
    Driver::Driver(QObject* object)
        :QObject(object)
    {}

    Driver::~Driver()
    {}

    void Driver::loadQml()
    {
        mQmlEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    }
}
