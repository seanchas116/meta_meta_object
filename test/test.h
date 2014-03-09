#pragma once
#include <QObject>

void addTestObject(QObject *obj);

#define DECLARE_TESTCLASS(CLASS)\
    namespace { auto _test_init_##CLASS = [](){ addTestObject(new CLASS); return 0; }(); }
