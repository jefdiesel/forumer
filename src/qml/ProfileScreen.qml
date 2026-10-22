import QtQuick
import QtQuick.Layouts

// profile & personas: the persona you post as, how it rotates, your alias,
// and what "post as" defaults to.
Page {
    id: profile

    required property var store
    property string message: ""

    title: "profile"
    subtitle: "your identity never leaves this device. people only ever see personas made from it - or nothing at all."

    Section {
        title: "current persona"
        caption: "who your next post is signed as"

        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            spacing: 14
            Text {
                textFormat: Text.PlainText
                text: profile.store.myPersona
                font.family: Ui.mono
                font.pixelSize: 28
                font.weight: Ui.weight.medium
                color: Ui.text
            }
            Text {
                textFormat: Text.PlainText
                Layout.fillWidth: true
                text: "a persona is a key made from your identity. anyone can check a post was signed by it; nobody can tell two of your personas belong to the same person."
                wrapMode: Text.WordWrap
                font.family: Ui.sans
                font.pixelSize: Ui.size.ui
                lineHeight: 1.35
                color: Ui.text2
            }
        }
    }

    Section {
        title: "rotation"
        caption: "when to switch to a fresh persona"

        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            spacing: 12
            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                Segmented {
                    Layout.preferredWidth: 420
                    options: [
                        { value: "keep", label: "keep" },
                        { value: "manual", label: "manual" },
                        { value: "auto", label: "auto · every post" }
                    ]
                    value: profile.store.rotationPolicy
                    onPicked: (v) => profile.store.call(profile.store.backend.chooseRotationPolicy(v))
                }
                FButton {
                    visible: profile.store.rotationPolicy === "manual"
                    text: "rotate now"
                    icon: "refresh"
                    implicitHeight: 46
                    onClicked: profile.store.call(profile.store.backend.rotatePersona(),
                                                  function () { profile.message = "new persona: " + profile.store.myPersona; })
                }
            }
            Text {
                textFormat: Text.PlainText
                Layout.fillWidth: true
                text: profile.store.rotationPolicy === "keep" ? "always the same persona, so people can follow what you write."
                    : profile.store.rotationPolicy === "auto" ? "every post gets a persona of its own, so no two of your posts can be linked."
                    : "the same persona until you press “rotate now”."
                wrapMode: Text.WordWrap
                font.family: Ui.mono
                font.pixelSize: Ui.size.caption
                color: Ui.text3
            }
            Notice { Layout.fillWidth: true; error: false; message: profile.message }
        }
    }

    Section {
        title: "alias"
        caption: "a display name, shown next to the persona"

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            spacing: 12
            FField {
                id: aliasField
                Layout.preferredWidth: 320
                placeholder: "e.g. night owl"
                text: profile.store.alias
                onAccepted: profile.store.call(profile.store.backend.chooseAlias(aliasField.text))
            }
            FButton {
                Layout.alignment: Qt.AlignBottom
                implicitHeight: 44
                text: "save"
                enabled: aliasField.text !== profile.store.alias
                onClicked: profile.store.call(profile.store.backend.chooseAlias(aliasField.text))
            }
            Item { Layout.fillWidth: true }
        }
    }

    Section {
        id: nameSection
        title: "logos name"
        caption: "use a Logos name you hold as your alias, and readers see it verified"

        readonly property bool isName: profile.store.isNameAlias(profile.store.alias)
        readonly property bool verified: isName
            && profile.store.verifiedNames[profile.store.alias + "|" + profile.store.myPersonaKey] === true
        // Keeps the keys already listed, so earlier personas stay verified.
        readonly property string command: "names set-text " + profile.store.alias
            + " forumer.persona \"" + (profile.store.myNameEntry || profile.store.myPersonaKey) + "\""

        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            spacing: 12
            NameTag { name: nameSection.verified ? profile.store.alias : "" }
            Text {
                textFormat: Text.PlainText
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                font.family: Ui.mono
                font.pixelSize: Ui.size.caption
                color: Ui.text3
                text: !nameSection.isName
                      ? "set your alias to a name you hold (lowercase, like jef) to link it"
                      : nameSection.verified
                        ? "posts under this alias show as your verified name. The name can be held publicly or anonymously; only this persona's key is listed on it."
                        : "list this persona on " + profile.store.alias + ", from the Names app or the names command line:"
            }
            RowLayout {
                visible: nameSection.isName && !nameSection.verified && profile.store.myPersonaKey.length > 0
                Layout.fillWidth: true
                spacing: 12
                Text {
                    textFormat: Text.PlainText
                    Layout.fillWidth: true
                    text: nameSection.command
                    wrapMode: Text.WrapAnywhere
                    font.family: Ui.mono
                    font.pixelSize: Ui.size.caption
                    color: Ui.text2
                }
                FButton {
                    text: "copy"
                    onClicked: {
                        nameClipboard.text = nameSection.command;
                        nameClipboard.selectAll();
                        nameClipboard.copy();
                        nameClipboard.text = "";
                    }
                }
            }
            TextEdit { id: nameClipboard; visible: false; textFormat: TextEdit.PlainText }
        }
    }

    Section {
        title: "post as, by default"
        caption: "you can still change it on each post"

        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            spacing: 12
            Segmented {
                Layout.preferredWidth: 420
                options: profile.store.disclosureOptions()
                value: profile.store.defaultDisclosure
                onPicked: (v) => profile.store.call(profile.store.backend.chooseDefaultDisclosure(v))
            }
            Text {
                textFormat: Text.PlainText
                Layout.fillWidth: true
                text: profile.store.disclosureHint(profile.store.defaultDisclosure)
                wrapMode: Text.WordWrap
                font.family: Ui.mono
                font.pixelSize: Ui.size.caption
                color: Ui.text3
            }
        }
    }
}
