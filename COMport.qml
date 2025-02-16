import MillingMachine

GradientRectangle{
    width: 300
    height: 125
    radius: 20

    GradientText{
        id: textCOMPort
        anchors.horizontalCenter: parent.horizontalCenter
        width: 125
        y: 25
        text: "COM-port №"
        fontName: montserratFont.name
    }

    GradientTextEdit{
        id: numberCOMPort
        anchors.left: textCOMPort.right
        anchors.leftMargin: 20
        y: 25
        text: "4"
        fontName: montserratFont.name
    }

    GradientButton{
        id: connect
        x: 17
        y: 50
        width: 125
        height: 50
        colorButton: "#539ef3"
        text: "Connect"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.openCOM(Number(numberCOMPort.text))
        }
    }

    GradientButton{
        id: disconnect
        x: 158
        y: 50
        width: 125
        height: 50
        colorButton: "#539ef3"
        text: "Disconnect"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.closeCOM()
        }
    }
}
