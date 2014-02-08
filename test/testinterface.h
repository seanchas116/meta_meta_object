#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

#include <QObject>

class TestInterfaceObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER mName NOTIFY nameChanged)
public:

    Q_INVOKABLE QString method(int a1, double a2, const QString &a3)
    {
        return a1 * a2 * a3.toInt();
    }

    Q_INVOKABLE QVariant variantMethod(const QVariant &a1, const QVariant &a2)
    {
        return a1.toString() + QString::number(a2.toInt());
    }

    Q_INVOKABLE int overloaded()
    {
        return 288;
    }

    Q_INVOKABLE int overloaded(int x)
    {
        return x * x;
    }

signals:
    void nameChanged(const QString &name);

private:
    QString mName;
};

class TestInterface : public QObject
{
    Q_OBJECT
public:
    explicit TestInterface(QObject *parent = 0);

private slots:
    void initTestCase();
    void test_call();
    void test_variatnCall();
    void test_overloadedCall();
    void test_property();
    void test_emitPropertyChanged();
    void cleanupTestCase();

};

#endif // TESTINTERFACE_H
