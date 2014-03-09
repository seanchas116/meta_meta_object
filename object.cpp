#include "object.h"
#include "metaobject.h"

namespace MetaMetaObject {

Object::Object(const std::shared_ptr<MetaObject> &metaObject, QObject *parent) :
    QObject(parent),
    mMetaObject(metaObject)
{
}

const QMetaObject *Object::metaObject() const
{
    return mMetaObject.get();
}

int Object::qt_metacall(QMetaObject::Call call, int index, void **argv)
{
    return mMetaObject->dynamicMetaCall(this, call, index, argv);
}

} // namespace FQI
