import QtQuick
import QtQuick.Effects

Item {
    property string text: "Текст"
    property int shadowHorizontalOffset: 2
    property int shadowVerticalOffset: 2
    property int fontText: 16
    property string fontName: "Timew New Roman"

    TextEdit{
        id: text
        anchors.centerIn: parent
        text: parent.text
        font.family: fontName
        font.pointSize: parent.fontText
        color: "white"
    }

    MultiEffect {
        id: textMulti
        anchors.fill: text
        source: text
        shadowBlur: 0.2
        shadowHorizontalOffset: parent.shadowHorizontalOffset
        shadowVerticalOffset: parent.shadowVerticalOffset
        shadowEnabled: true
    }
}
