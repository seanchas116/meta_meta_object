#include "metaobject.h"
#include "object.h"

namespace FQI {

MetaObject::MetaObject(
    const QVector<uint8_t> &stringData, const QVector<uint> &data,
    const QVector<MethodInfo> &methodInfos, const QVector<PropertyInfo> &propertyInfos) :
    mStringData(stringData),
    mData(data),
    mMethods(methodInfos),
    mProperties(propertyInfos)
{
    d.superdata = &QObject::staticMetaObject;
    d.stringdata = reinterpret_cast<const QByteArrayData *>(mStringData.data());
    d.data = mData.data();
    d.static_metacall = nullptr;
    d.relatedMetaObjects = nullptr;
    d.extradata = nullptr;
}

int MetaObject::dynamicMetaCall(Object *obj, QMetaObject::Call call, int index, void **argv)
{
    index = obj->QObject::qt_metacall(call, index, argv);
    if (index < 0) {
        return index;
    }

    switch (call) {
    case QMetaObject::InvokeMetaMethod: {
        if (index < mMethods.size()) {
            const auto &methodInfo = mMethods[index];
            if (methodInfo.isSignal) {
                QMetaObject::activate(obj, this, index, argv);
            }
            else {
                QVariantList args;
                std::transform(argv + 1, argv + 1 + methodInfo.arity, std::back_inserter(args), [](void *arg) {
                    return *reinterpret_cast<QVariant *>(arg);
                });
                auto result = methodInfo.method(obj, args);
                if (argv[0]) {
                    *static_cast<QVariant *>(argv[0]) = result;
                }
            }
        }
        index -= mMethods.size();
    } break;

    case QMetaObject::ReadProperty: {
        if (index < mProperties.size()) {
            *static_cast<QVariant *>(argv[0]) = mProperties[index].getter(obj, {});
        }
        index -= mProperties.size();
    } break;

    case QMetaObject::WriteProperty: {
        if (index < mProperties.size()) {
            mProperties[index].setter(obj, {*static_cast<QVariant *>(argv[0])});
        }
        index -= mProperties.size();
    } break;

    case QMetaObject::ResetProperty:
    case QMetaObject::QueryPropertyDesignable:
    case QMetaObject::QueryPropertyScriptable:
    case QMetaObject::QueryPropertyStored:
    case QMetaObject::QueryPropertyEditable:
    case QMetaObject::QueryPropertyUser: {
        index -= mProperties.size();
    } break;

    case QMetaObject::RegisterPropertyMetaType: {
        if (index < mProperties.size()) {
            *static_cast<int *>(argv[0]) = -1;
        }
        index -= mProperties.size();
    } break;

    default: break;
    }

    return index;
}

} // namespace ForeignQObject
