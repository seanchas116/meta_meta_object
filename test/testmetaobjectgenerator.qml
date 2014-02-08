import QtQuick 2.2
import QtTest 1.0
import TestObject 1.0

TestCase {
    name: "TestMetaObjectGenerator"
    property bool signalEmitted: false

    TestObject {
        id: testObject
        onChanged: {
            parent.signalEmitted = true
        }
    }
    TestObject {
        id: testObject2
        name: testObject.name
    }

    function testMethod() {
        compare(testObject.method(1, "foo"), "1foo", "method")
    }
    function testOverloadedMethod() {
        compare(testObject.overloadedMethod(3, 5), 15, "overloaded method")
    }
    function testProperty() {
        testObject.name = "hogehoge"
        compare(testObject.name, "hogehoge", "property")
    }
    function testPropertyBinding() {
        testObject.name = "piyo"
        compare(testObject2.name, "piyo", "property binding")
    }
    function testSignal() {
        signalEmitted = false
        testObject.changed()
        compare(signalEmitted, true, "signal")
    }
}
