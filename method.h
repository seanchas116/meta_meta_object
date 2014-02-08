#pragma once

#include <QVariant>
#include <functional>

namespace FQI {

class Object;

using Method = std::function<QVariant(Object *obj, const QVariantList &args)>;

}
