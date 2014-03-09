#include "../object.h"
#include "../metaobjectbuilder.h"
#include "test.h"

#include <QtTest>
#include <QJSEngine>
#include <QSignalSpy>

using namespace MetaMetaObject;

class SampleObject : public Object
{
public:
    SampleObject(const std::shared_ptr<MetaObject> &metaObject, QObject *parent) :
        Object(metaObject, parent)
    {}

    QString name;
};

class TestMetaObjectGenerator : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
        MetaObjectBuilder generator;

        auto method = [](Object *object, const QVariantList &args) {
            return args[0].toString() + args[1].toString();
        };
        auto method2 = [](Object *object, const QVariantList &args) {
            return args[0].toInt() * args[1].toInt();
        };
        auto setter = [](Object *object, const QVariantList &args) {
            auto sampleObject = static_cast<SampleObject *>(object);
            auto str = args[0].toString();
            if (sampleObject->name != str) {
                sampleObject->name = str;
                auto propertyIndex = sampleObject->metaObject()->indexOfProperty("name");
                auto property = sampleObject->metaObject()->property(propertyIndex);
                property.notifySignal().invoke(sampleObject, Q_ARG(QVariant, str));
            }
            return QVariant();
        };
        auto getter = [](Object *object, const QVariantList &args) {
            return static_cast<SampleObject *>(object)->name;
        };

        generator.setClassName("SampleObject");
        generator.addMethod("method", 2, method);
        generator.addProperty("name", getter, setter);
        generator.addMethod("method2", 2, method2, true);

        auto metaObject = generator.create();

        mSampleObject = new SampleObject(metaObject, this);

        mJsEngine = new QJSEngine(this);
        auto objectValue = mJsEngine->newQObject(mSampleObject);
        mJsEngine->globalObject().setProperty("sample", objectValue);

        for (int i = 0; i < metaObject->methodCount(); ++i) {
            qDebug() << metaObject->method(i).methodSignature();
        }

        for (int i = 0; i < metaObject->propertyCount(); ++i) {
            qDebug() << metaObject->property(i).name() << metaObject->property(i).typeName();
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
        QCOMPARE(spy.takeFirst().at(0).toString(), QString("foobar"));
    }

private:

    QObject *mSampleObject;
    QJSEngine *mJsEngine;
};

DECLARE_TESTCLASS(TestMetaObjectGenerator)

#include "testmetaobjectgenerator.moc"
