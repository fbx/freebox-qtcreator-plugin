import QtQuick 2.2
import fbx.ui.page 1.0
import fbx.ui.control 1.0

Page {
    title: "Page Zero"

    Column {
        anchors.centerIn: parent

        Text {
            color: "white"
            font.pixelSize: 50
            text: "Hi, I'm page 0"
        }

        Row {
            Button {
                id: p0

                focus: true

                text: "Stack page 0"
                onClicked: stack.push("page0.qml")

                KeyNavigation.right: p1
            }

            Button {
                id: p1

                text: "Stack page 1"
                onClicked: stack.push("page1.qml")

                KeyNavigation.left: p0
            }
        }
    }
}
