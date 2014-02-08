#include "../metaobjectgenerator.h"
#include "../metaobject.h"
#include "testmetaobjectgenerator.h"

#include <QCoreApplication>
#include <QtTest>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QObjectList testObjects = {new TestMetaObjectGenerator};

    int result = 0;

    for (auto test : testObjects) {
        result += QTest::qExec(test);
    }

    return result;
}
