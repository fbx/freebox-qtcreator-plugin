import QtQuick 2.0
import fbx.ui.settings 1.0
import fbx.ui.control 1.0

Page {
    title: "Compte"

    Entry {
        text: "Login"
        TextInput {
            placeholderText: "user@example.com"
        }
    }

    Entry {
        text: "Password"
        TextInput {
            placeholderText: "****"
            echoMode: TextInput.PasswordEchoOnEdit
        }
    }
}
