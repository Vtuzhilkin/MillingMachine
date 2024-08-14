import QtQuick 2.15

Rectangle {
    id: button
    property string button_text: "Button"
    signal buttonClicked()
    property color defaultColor: "#FFA500"
    property color pressColor: "#CC8400" //"#8B4513"
    property color enterColor: "#FFC733" //"#FFD580"
    property real defaultFontSize: height / 3

    radius: 10
    border.width: 1
    color: defaultColor
    Text {

        anchors.centerIn: parent
        text: button_text
        font.pixelSize: parent.defaultFontSize
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            button.color = enterColor //"#FFD580"
        }
        onExited: {
            button.color = defaultColor //"#FFA500"
        }
        onClicked: {
            button.buttonClicked()
            button.color = enterColor //"#FFD580"
        }
        onPressed: {
            button.color =  pressColor //"#8B4513"
        }
    }
}
