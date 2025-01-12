import QtQuick
import QtQuick.Effects

Item {
    property color colorButton: "grey"
    property string text: "Кнопка"
    property int radius: 20
    property int fontText: 16
    property string fontName: "Calibre"

    signal clicked()
    signal released()

    function onHovered(pressed){
        button.shadowHorizontalOffset = 0
        button.shadowVerticalOffset = 0
        textMulti.shadowHorizontalOffset = 0
        textMulti.shadowVerticalOffset = 0
        if(pressed){
            button.colorStart = "black"
            button.colorEnd = "white"
        }
    }

    function offHovered(){
        button.shadowHorizontalOffset = 5
        button.shadowVerticalOffset = 5
        textMulti.shadowHorizontalOffset = 2
        textMulti.shadowVerticalOffset = 2
        button.colorStart = "white"
        button.colorEnd = "black"
    }

    GradientRectangle{
        id: button
        colorCenter: parent.colorButton
        anchors.fill: parent
        radius: parent.radius
    }

    GradientText{
        id: textMulti
        anchors.centerIn: button
        text: parent.text
        fontText: parent.fontText
        fontName: parent.fontName
    }

    MouseArea{
        id: buttonMouse
        anchors.fill: button
        hoverEnabled: true
        onHoveredChanged: {
            if (containsMouse) {
                parent.onHovered(false)
            }else{
                parent.offHovered()
            }
        }
        onPressed: {
            button.colorStart = "black"
            button.colorEnd = "white"
        }
        onReleased:{
            button.colorStart = "white"
            button.colorEnd = "black"
            parent.clicked()
            parent.released()
        }
    }
}
