import QtQuick 2.4
import fbx.application 1.0

Application {
    Text {
        focus: true
        anchors.centerIn: parent

        text: "Main entry point"
        color: "white"
        font.pixelSize: 40

        Keys.onPressed: {
            if (event.key == Qt.Key_Back)
                Qt.quit();
        }
    }
}
