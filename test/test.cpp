#include "test.h"
#include <QtTest>
#include <QCoreApplication>

QObjectList *testObjects = nullptr;

void addTestObject(QObject *obj)
{
    if (!testObjects) {
        testObjects = new QObjectList;
    }
    *testObjects << obj;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a);

    int result = 0;

    for (auto test : *testObjects) {
        result += QTest::qExec(test);
    }

    return result;
}
