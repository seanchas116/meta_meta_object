#pragma once

#include <QVariant>
#include <functional>

namespace MetaMetaObject {

class Object;

using Method = std::function<QVariant(Object *obj, const QVariantList &args)>;

}
