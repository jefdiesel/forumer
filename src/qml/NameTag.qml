import QtQuick

// A verified Logos name: the post's alias is a name whose record lists the
// persona that signed it. Drawn as a chip, apart from the author text, so an
// alias typed to look like one stays plain text. Hidden when `name` is "".
Rectangle {
    id: tag

    property string name: ""

    visible: name.length > 0
    implicitWidth: label.implicitWidth + 12
    implicitHeight: label.implicitHeight + 4
    radius: 3
    color: "transparent"
    border.width: 1
    border.color: Ui.accent

    Text {
        id: label
        anchors.centerIn: parent
        textFormat: Text.PlainText
        text: "✓ " + tag.name + " · logos name"
        font.family: Ui.mono
        font.pixelSize: Ui.size.caption
        color: Ui.accent
    }
}
