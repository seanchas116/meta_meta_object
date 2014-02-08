#include "metaobjectgenerator.h"
#include "metaobject.h"
#include <QVector>
#include <QQueue>

namespace FQI {

namespace {

// from qmetaobject_p.h in Qt 5.2.0

enum PropertyFlags  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
//     Override = 0x00000200,
    Constant = 0x00000400,
    Final = 0x00000800,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000,
    Notify = 0x00400000,
    Revisioned = 0x00800000
};

enum MethodFlags  {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask

    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodConstructor = 0x0c,
    MethodTypeMask = 0x0c,

    MethodCompatibility = 0x10,
    MethodCloned = 0x20,
    MethodScriptable = 0x40,
    MethodRevisioned = 0x80
};

}

class MetaObjectGenerator::StringPool
{
public:
    int intern(const QByteArray &str) {
        int size = mStrings.size();
        for (int i = 0; i < size; ++i) {
            if (mStrings[i] == str)
                return i;
        }
        mStrings << str;
        return size;
    }

    QVector<uint8_t> toMetaStringData()
    {
        int count = mStrings.size();
        int size = sizeof(QByteArrayData) * count;
        for (const auto &str : mStrings) {
            size += str.size() + 1;
        }
        QVector<uint8_t> data(size);
        auto arrayDatas = reinterpret_cast<QArrayData *>(data.data());
        auto stringData = reinterpret_cast<char *>(data.data() + sizeof(QByteArrayData) * count);

        int stringOffset = 0;

        for (int i = 0; i < count; ++i) {
            const auto &string = mStrings[i];
            auto size = string.size();

            // write array data
            auto arrayDataOffset = stringOffset + sizeof(QByteArrayData) * (count - i);
            arrayDatas[i] = QArrayData { Q_REFCOUNT_INITIALIZE_STATIC, size, 0, 0, qptrdiff(arrayDataOffset) };

            // write string data
            strcpy(stringData + stringOffset, string.data());
            stringOffset += string.size() + 1;
        }

        return data;
    }

private:
    QList<QByteArray> mStrings;
};

MetaObjectGenerator::MetaObjectGenerator()
{
}

void MetaObjectGenerator::setClassName(const QByteArray &className)
{
    mClassName = className;
}

void MetaObjectGenerator::addSignal(const QByteArray &name, int arity)
{
    Q_ASSERT(!name.isEmpty());

    MethodDef method;
    method.name = name;
    method.arity = arity;
    mSignals << method;
}

void MetaObjectGenerator::addMethod(const QByteArray &name, int arity, const Method &method, bool isSlot)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(arity >= 0);
    Q_ASSERT(method);

    MethodDef methodDef;
    methodDef.name = name;
    methodDef.arity = arity;
    methodDef.method = method;
    if (isSlot) {
        mSlots << methodDef;
    } else {
        mMethods << methodDef;
    }
}

void MetaObjectGenerator::addProperty(const QByteArray &name, const Method &getter, const Method &setter)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(getter);

    PropertyDef property;
    property.name = name;
    property.getter = getter;
    property.setter = setter;
    property.notifySignalName = name + "Changed";
    addSignal(property.notifySignalName, 1);
    mProperties << property;
}

std::shared_ptr<MetaObject> MetaObjectGenerator::create() const
{
    StringPool stringPool;
    auto metaData = writeMetaData(stringPool);
    auto metaStringData = stringPool.toMetaStringData();

    QVector<MetaObject::MethodInfo> methodInfos;
    QVector<MetaObject::PropertyInfo> propertyInfos;

    auto toMethodInfo = [](const MethodDef &signal, bool isSignal) {
        MetaObject::MethodInfo info;
        info.method = signal.method;
        info.arity = signal.arity;
        info.isSignal = isSignal;
        return info;
    };

    for (const auto &signal : mSignals) {
        methodInfos << toMethodInfo(signal, true);
    }
    for (const auto &slot : mSlots) {
        methodInfos << toMethodInfo(slot, false);
    }
    for (const auto &method : mMethods) {
        methodInfos << toMethodInfo(method, false);
    }

    for (const auto &property : mProperties) {
        MetaObject::PropertyInfo info;
        info.getter = property.getter;
        info.setter = property.setter;
        info.notifySignalId = notifySignalIndex(property);
        propertyInfos << info;
    }

    auto metaObject = std::make_shared<MetaObject>(metaStringData, metaData, methodInfos, propertyInfos);

    return metaObject;
}

QVector<uint> MetaObjectGenerator::writeMetaData(StringPool &stringPool) const
{
    int methodDataSize = 0;

    for (const auto &method : mMethods) {
        methodDataSize += 5 + 1 + method.arity * 2;
    }

    QVector<uint> metaData;

    auto markIndex = [&]() {
        auto index = metaData.size();
        metaData << 0;
        return index;
    };

    auto writeCurrentPos = [&](int markedIndex) {
        metaData[markedIndex] = metaData.size();
    };

    // write header //

    metaData << 7; // revision
    metaData << stringPool.intern(mClassName); // classname
    metaData << 0 << 0; // classinfo

    // methods
    metaData << (mSignals.size() + mSlots.size() + mMethods.size());
    int methodInfoPosIndex = markIndex();

    // properties
    metaData << mProperties.size();
    int propertyInfoPosIndex = markIndex();

    metaData << 0 << 0; // enums
    metaData << 0 << 0; // constructors
    metaData << 0; // flags
    metaData << mSignals.size(); // signal count

    // write method info //

    writeCurrentPos(methodInfoPosIndex);

    QQueue<int> parameterInfoPosIndexes;

    auto addMethodInfo = [&](const MethodDef &method, bool isSignal, bool isSlot) {
        metaData << stringPool.intern(method.name); // name
        metaData << method.arity; // argc
        parameterInfoPosIndexes.enqueue(markIndex()); // parameters
        metaData << 2; // tag
        int flags = AccessPublic;
        if (isSignal) {
            flags |= MethodSignal;
        }
        if (isSlot) {
            flags |= MethodSlot;
        }
        metaData << flags; // flags
    };

    for (const auto &signal : mSignals) {
        addMethodInfo(signal, true, false);
    }
    for (const auto &slot : mSlots) {
        addMethodInfo(slot, false, true);
    }
    for (const auto &method : mMethods) {
        addMethodInfo(method, false, false);
    }

    auto addParametersInfo = [&](const MethodDef &method) {
        writeCurrentPos(parameterInfoPosIndexes.dequeue());
        metaData << QMetaType::QVariant;
        for (int i = 0; i < method.arity; ++i) {
            metaData << QMetaType::QVariant;
        }
        for (int i = 0; i < method.arity; ++i) {
            metaData << stringPool.intern("");
        }
    };

    for (const auto &signal : mSignals) {
        addParametersInfo(signal);
    }
    for (const auto &slot : mSlots) {
        addParametersInfo(slot);
    }
    for (const auto &method : mMethods) {
        addParametersInfo(method);
    }

    // write property info //

    writeCurrentPos(propertyInfoPosIndex);

    for (const auto &property : mProperties) {
        metaData << stringPool.intern(property.name);
        metaData << QMetaType::QVariant;
        int flags = Notify | ResolveEditable | Stored | Scriptable | Designable | Readable;
        if (property.setter) {
            flags |= Writable;
        }
        metaData << flags;
    }

    for (const auto &property : mProperties) {
        auto signalIndex = notifySignalIndex(property);
        Q_ASSERT(signalIndex >= 0);
        metaData << signalIndex;
    }

    // end of data //

    metaData << 0;

    return metaData;
}

int MetaObjectGenerator::notifySignalIndex(const PropertyDef &property) const
{
    for (int i = 0; i < mSignals.size(); ++i) {
        if (mSignals[i].arity == 1 && mSignals[i].name == property.notifySignalName)
            return i;
    }
    return -1;
}

} // namespace ForeignQObject
