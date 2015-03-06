import QtQuick 2.4
import fbx.application 1.0
import fbx.ui.settings 1.0

Application {
    View {
        focus: true
        anchors.fill: parent
        baseUrl: Qt.resolvedUrl("settings/")
        initialPage: "main.qml"
    }
}
