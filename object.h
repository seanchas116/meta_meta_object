#pragma once
#include <QObject>

namespace FQI {

class MetaObject;

class Object : public QObject
{
public:
    explicit Object(const std::shared_ptr<MetaObject> &metaObject, QObject *parent = 0);

    const QMetaObject *metaObject() const override;
    int qt_metacall(QMetaObject::Call call, int index, void **argv) override;

private:
    std::shared_ptr<MetaObject> mMetaObject;
};

} // namespace FQI
