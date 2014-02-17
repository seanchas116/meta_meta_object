#pragma once
#include <QObject>

class QJSEngine;

class TestMetaObjectGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TestMetaObjectGenerator(QObject *parent = 0);

private slots:

    void initTestCase();
    void test_className();
    void test_method();
    void test_method2();
    void test_property();
    void test_propertyChangeSignal();
    void cleanupTestCase();

private:

    QObject *mSampleObject;
    QJSEngine *mJsEngine;
};
