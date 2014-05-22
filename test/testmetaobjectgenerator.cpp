#include "../object.h"
#include "../foreignclass.h"
#include "../metaobject.h"
#include "test.h"

#include <QtTest>
#include <QJSEngine>
#include <QSignalSpy>

using namespace MetaMetaObject;

class SampleObject : public Object
{
public:
    SampleObject(const SP<ForeignClass> &klass, QObject *parent) :
        Object(klass, parent)
    {}

    QString name;
};

class SampleClass : public ForeignClass
{
public:

    struct Id
    {
        enum Ids
        {
            method, method2, name, nameChanged
        };
    };

    SampleClass()
    {
        setClassName("SampleObject");
        addMethod("method", Id::method, 2);
        addMethod("method2", Id::method2, 2);
        addProperty("name", Id::name, Property::Flag::Writable | Property::Flag::Readable, true, Id::nameChanged);
        addSignal("nameChanged", Id::nameChanged, 1);
    }

    QVariant callMethod(Object *obj, size_t id, const QVariantList &args) override
    {
        Q_UNUSED(obj);
        switch (id) {
        case Id::method:
            return args[0].toString() + args[1].toString();
        case Id::method2:
            return args[0].toInt() * args[1].toInt();
        default:
            return QVariant();
        }
    }
    QVariant getProperty(Object *obj, size_t id) override
    {
        switch (id) {
        case Id::name:
            return static_cast<SampleObject *>(obj)->name;
        default:
            return QVariant();
        }
    }
    void setProperty(Object *obj, size_t id, const QVariant &value) override
    {
        switch (id) {
        case Id::name: {
            auto sampleobj = static_cast<SampleObject *>(obj);
            if (sampleobj->name != value.toString()) {
                sampleobj->name = value.toString();
                emitSignal(obj, Id::nameChanged, {value});
            }
            break;
        }
        default:
            break;
        }
    }
};

class SampleSubclass : public ForeignClass
{
public:

    struct Id
    {
        enum Ids
        {
            method
        };
    };

    SampleSubclass(const SP<ForeignClass> &superclass) : ForeignClass(superclass)
    {
        addMethod("method", Id::method, 1);
    }

    QVariant callMethod(Object *obj, size_t id, const QVariantList &args) override
    {
        Q_UNUSED(obj);
        Q_UNUSED(args);
        switch (id) {
        case Id::method:
            return "override";
        default:
            return QVariant();
        }
    }
};

class TestMetaObjectGenerator : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
        auto klass = makeSP<SampleClass>();
        auto subclass = makeSP<SampleSubclass>(klass);

        mSampleObject = new SampleObject(klass, this);
        mSampleSubObject = new SampleObject(subclass, this);

        mJsEngine = new QJSEngine(this);
        mJsEngine->globalObject().setProperty("sample", mJsEngine->newQObject(mSampleObject));
        mJsEngine->globalObject().setProperty("sampleSub", mJsEngine->newQObject(mSampleSubObject));

        auto metaobj = klass->metaObject();

        for (int i = 0; i < metaobj->methodCount(); ++i) {
            qDebug() << metaobj->method(i).methodSignature();
        }

        for (int i = 0; i < metaobj->propertyCount(); ++i) {
            qDebug() << metaobj->property(i).name() << metaobj->property(i).typeName();
        }
    }

    void test_className()
    {
        QCOMPARE(QString(mSampleObject->metaObject()->className()), QString("SampleObject"));
    }

    void test_method()
    {
        auto result = mJsEngine->evaluate("sample.method(123, 'foo')").toString();
        QCOMPARE(result, QString("123foo"));
    }

    void test_subclassOverridden()
    {
        auto result = mJsEngine->evaluate("sampleSub.method(123)").toString();
        QCOMPARE(result, QString("override"));
    }

    void test_subclassNonOverridden()
    {
        auto result = mJsEngine->evaluate("sampleSub.method2(12, 24)").toInt();
        QCOMPARE(result, 288);
    }

    void test_method2()
    {
        auto result = mJsEngine->evaluate("sample.method2(12, 24)").toInt();
        QCOMPARE(result, 288);
    }

    void test_property()
    {
        auto result = mJsEngine->evaluate("sample.name = 'hogehoge'; sample.name").toString();
        QCOMPARE(result, QString("hogehoge"));
    }

    void test_propertyChangeSignal()
    {
        QSignalSpy spy(mSampleObject, SIGNAL(nameChanged(QVariant)));
        mSampleObject->setProperty("name", "foobar");
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.takeFirst(), QVariantList {QString("foobar") });
    }

private:

    QObject *mSampleObject;
    QObject *mSampleSubObject;
    QJSEngine *mJsEngine;
};

DECLARE_TESTCLASS(TestMetaObjectGenerator)

#include "testmetaobjectgenerator.moc"
