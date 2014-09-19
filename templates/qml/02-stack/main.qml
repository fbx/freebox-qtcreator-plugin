import QtQuick 2.2
import fbx.application 1.0
import fbx.ui.page 1.0
import fbx.ui.layout 1.0

Application {
    Background {
        background: "player"
    }

    Breadcrumb {
        id: breadcrumb

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        stack: pageStack

        KeyNavigation.down: pageStack
    }

    Stack {
        id: pageStack

        anchors {
            top: breadcrumb.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        focus: true

        initialPage: "page0.qml"
        baseUrl: Qt.resolvedUrl("pages/")

        KeyNavigation.up: breadcrumb
    }
}
