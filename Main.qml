import QtQuick
import QtQuick.Effects
import QtQuick.Controls

Window {
    id: mainWindow
    visible: true
    visibility: Window.Maximized
    flags: Qt.FramelessWindowHint | Qt.Window

    onVisibilityChanged: {
        if (mainWindow.visibility === Window.Windowed) {
            mainWindow.visibility = Window.Maximized;
        }
    }

    FontLoader {
        id: montserratFont
        source: "qrc:/fonts/Montserrat-Medium.ttf"
    }

    Rectangle{
        id: fullRectangle
        anchors.fill: parent
        color: "#edece8"
    }
    ShaderEffectSource{
        id: fullRectangleSource
        sourceItem: fullRectangle
        anchors.fill: fullRectangle
        hideSource: true;
    }

    ShaderEffect{
        anchors.fill: fullRectangle
        property variant source: fullRectangleSource
        property color colorStart: "#edece8"  // Начальный цвет градиента
        property color colorEnd: "#898e94"   // Конечный цвет градиента
        fragmentShader: "qrc:/shaders/gradient.frag.qsb"
    }

    Column{
        x: 20
        y: 50
        spacing: 20
        PositionButtons{
            id: rectPosition
        }

        RuleButtons{
            id: rectButtons
        }

        COMport{
            id: rectCOMPort
        }

        Status{
            id: rectStatus
        }
    }
    GradientRectangle{
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        colorStart: "lightgrey"
        colorCenter: "grey"
        colorEnd: "darkgrey"
        height: 30
        radius: 5
        GradientButton{
            id: hideButton
            anchors.top: parent.top
            anchors.right: closeButton.left
            colorButton: "red"
            text: "_"
            width: 30
            height: 30
            radius: 5
            onReleased:{
                visibility = Window.Minimized
            }
        }
        GradientButton{
            id: closeButton
            anchors.top: parent.top
            anchors.right: parent.right
            colorButton: "red"
            text: "X"
            width: 30
            height: 30
            radius: 5
            onReleased:{
                close()
            }
        }
    }

    GradientRectangle{
        id: rectScroll
        x: 340
        y: 50
        width: 350
        height: 765
        radius: 20
        GradientText{
            id: rectScrollText
            y: 10
            anchors.horizontalCenter: parent.horizontalCenter
            width: 100
            height: 20
            fontText: 22
            text: "G-code Listing"
        }
    }

    ScrollView{
        id: scrollViewCoordinates
        anchors.centerIn: rectScroll
        width: rectScroll.width
        height: rectScroll.height-3*rectScroll.radius
        TextArea{
            id: listCommands
            font.pixelSize: 22
            background: null
            font.family: montserratFont.name
            color: "white"
        }
    }

    MultiEffect {
        id: textScrollMulti
        anchors.fill: scrollViewCoordinates
        source: scrollViewCoordinates
        shadowBlur: 0.2
        shadowHorizontalOffset: 2
        shadowVerticalOffset: 2
        shadowEnabled: true
    }

    Window3D{
        x: 710
        y: 50
    }
}
