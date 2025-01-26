import QtQuick
import QtQuick.Effects

Item {
    property alias text: textEdit.text
    property int shadowHorizontalOffset: 2
    property int shadowVerticalOffset: 2
    property int fontText: 16
    property string fontName: "Timew New Roman"

    TextEdit{
        id: textEdit
        anchors.centerIn: parent
        font.family: fontName
        font.pointSize: parent.fontText
        color: "white"
    }

    MultiEffect {
        id: textMulti
        anchors.fill: textEdit
        source: textEdit
        shadowBlur: 0.2
        shadowHorizontalOffset: parent.shadowHorizontalOffset
        shadowVerticalOffset: parent.shadowVerticalOffset
        shadowEnabled: true
    }
}
