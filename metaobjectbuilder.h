#pragma once
#include "method.h"
#include <QMetaObject>
#include <QMetaMethod>
#include "metaobject.h"

namespace FQI {

class Object;

class MetaObjectBuilder
{
public:

    MetaObjectBuilder();

    void setClassName(const QByteArray &className);
    void addSignal(const QByteArray &name, int arity);
    void addMethod(const QByteArray &name, int arity, const Method &method, bool isSlot = false);
    void addProperty(const QByteArray &name, const Method &getter, const Method &setter = Method());

    std::shared_ptr<MetaObject> create() const;

private:

    struct MethodDef
    {
        QByteArray name;
        Method method;
        int arity = 0;
    };

    struct PropertyDef
    {
        QByteArray name;
        Method getter;
        Method setter;
        QByteArray notifySignalName;
    };

    class StringPool;

    QVector<uint> writeMetaData(StringPool &stringPool) const;
    int notifySignalIndex(const PropertyDef &property) const;

    QByteArray mClassName;
    QList<MethodDef> mSignals;
    QList<MethodDef> mSlots;
    QList<MethodDef> mMethods;
    QList<PropertyDef> mProperties;
};

} // namespace ForeignQObject
