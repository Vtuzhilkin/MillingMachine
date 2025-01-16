import MillingMachine

GradientRectangle{
    width: 300
    height: 125
    radius: 20

    GradientText{
        id: textCOMPort
        anchors.horizontalCenter: parent.horizontalCenter
        y: 25
        text: "COM-port"
        fontName: montserratFont.name
    }

    GradientButton{
        id: openCOM
        x: 17
        y: 50
        width: 125
        height: 50
        colorButton: "#539ef3"
        text: "Open"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.openCOM(3)
        }
    }

    GradientButton{
        id: closeCOM
        x: 158
        y: 50
        width: 125
        height: 50
        colorButton: "#539ef3"
        text: "Close"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.closeCOM()
        }
    }
}
