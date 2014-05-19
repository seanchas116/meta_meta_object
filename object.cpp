#include "object.h"
#include "metaobject.h"
#include "foreignclass.h"

namespace MetaMetaObject {

Object::Object(const SP<ForeignClass> &klass, QObject *parent) :
    QObject(parent),
    mForeignClass(klass)
{
}

const QMetaObject *Object::metaObject() const
{
    return mForeignClass->metaObject().get();
}

int Object::qt_metacall(QMetaObject::Call call, int index, void **argv)
{
    return mForeignClass->metaObject()->dynamicMetaCall(this, call, index, argv);
}

} // namespace FQI
