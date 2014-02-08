#include "testinterface.h"

#include "../interface.h"
#include <QtTest>

using namespace FQI;

TestInterface::TestInterface(QObject *parent) :
    QObject(parent)
{
}

void TestInterface::initTestCase()
{

}

void TestInterface::test_call()
{
    TestInterfaceObject object;
    Interface interface(&object);

    auto result = interface.call("method", {16, 2.5, "361"}).toDouble();
    QCOMPARE(result, 16 * 2.5 * 361);
}

void TestInterface::test_variatnCall()
{
    TestInterfaceObject object;
    Interface interface(&object);

    auto result = interface.call("variantMethod", {"hogepiyo", 467}).toString();
    QCOMPARE(result, QString("hogepiyo467"));
}

void TestInterface::test_overloadedCall()
{
    TestInterfaceObject object;
    Interface interface(&object);

    auto result = interface.call("overloaded", {3}).toInt();
    QCOMPARE(result, 9);
}

void TestInterface::test_property()
{
    TestInterfaceObject object;
    Interface interface(&object);

    auto result = interface.setProperty("name", "foobar").property("name");
    QCOMPARE(result, QString("foobar"));
}

void TestInterface::test_emitPropertyChanged()
{
    TestInterfaceObject object;
    Interface interface(&object);
    QSignalSpy spy(&object, SIGNAL(nameChanged(QString)));

    interface.emitPropertyChanged("name");
    QCOMPARE(spy.count(), 1);
}

void TestInterface::cleanupTestCase()
{

}
