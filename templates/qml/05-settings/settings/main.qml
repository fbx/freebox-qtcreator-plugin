import QtQuick 2.0
import fbx.ui.settings 1.0
import fbx.ui.control 1.0

Page {
    title: "Réglages"

    Section {
        text: "Compte"
    }

    LinkButton {
        text: "Associer un compte"
        onClicked: push("login.qml");
        info: "Permet d'associer un compte à votre application"
    }

    Entry {
        text: "Truc"
        info: "Active le truc secret"

        Switch {
            id: truc
            checked: false
        }
    }

    Entry {
        visible: truc.checked
        text: "Chose"
        info: "Active un autre truc"

        Switch {
            id: chose
            checked: false
        }
    }

    Entry {
        enabled: chose.checked
        text: "Bidule"
        info: "Fait quelque chose"

        Switch {
        }
    }
}
