#pragma once
#include <QMetaObject>
#include <QVector>
#include "method.h"

namespace ForeignQObject {

class MetaObject : public QMetaObject
{
public:

    struct MethodInfo
    {
        Method method;
        int arity = 0;
        bool isSignal = false;
    };

    struct PropertyInfo
    {
        Method getter;
        Method setter;
        int notifySignalId = -1;
    };

    MetaObject(
        const QVector<uint8_t> &stringData, const QVector<uint> &data,
        const QVector<MethodInfo> &methodInfos, const QVector<PropertyInfo> &propertyInfos);

    int dynamicMetaCall(Object *obj, QMetaObject::Call call, int index, void **argv);

private:

    QVector<uint8_t> mStringData;
    QVector<uint> mData;
    QVector<MethodInfo> mMethods;
    QVector<PropertyInfo> mProperties;
};

} // namespace ForeignQObject
