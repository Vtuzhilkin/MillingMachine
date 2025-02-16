import QtQuick
import MillingMachine

GradientRectangle{
    width: 300
    height: 350
    radius: 20

    Item {
        id: keyboard
        focus: true
        anchors.fill: parent
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Down) {
                minusX.onHovered(true)
            }
            if(event.key === Qt.Key_Up){
                plusX.onHovered(true)
            }
            if (event.key === Qt.Key_Left) {
                plusY.onHovered(true)
            }
            if(event.key === Qt.Key_Right){
                minusY.onHovered(true)
            }
            if (event.key === Qt.Key_A) {
                plusZ.onHovered(true)
            }
            if(event.key === Qt.Key_Z){
                minusZ.onHovered(true)
            }
        }
        Keys.onReleased: (event) =>{
            if (event.key === Qt.Key_Down) {
                minusX.offHovered()
                minusX.clicked()
            }
            if(event.key === Qt.Key_Up){
                plusX.offHovered()
                plusX.clicked()
            }
            if (event.key === Qt.Key_Left) {
                plusY.offHovered()
                plusY.clicked()
            }
            if(event.key === Qt.Key_Right){
                minusY.offHovered()
                minusY.clicked()
            }
            if (event.key === Qt.Key_A) {
                plusZ.offHovered()
                plusZ.clicked()
            }
            if(event.key === Qt.Key_Z){
                minusZ.offHovered()
                minusZ.clicked()
            }
        }
    }

    GradientText{
        id: textPosition
        x: 20
        y: 20
        anchors.horizontalCenter: rectPosition.horizontalCenter
        text: "Position: X" + Number(MillingMachine.xCoordinate).toFixed(2).toString() +
                        "Y" + Number(MillingMachine.yCoordinate).toFixed(2).toString() +
                        "Z" + Number(MillingMachine.zCoordinate).toFixed(2).toString()
        fontName: montserratFont.name
    }

    GradientText{
        id: textX
        x: 20
        y: 40
        width: 140
        height: 50
        text: "Move by X"
        fontName: montserratFont.name
    }

    GradientButton{
        id: plusX
        x: 170
        y: 40
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "+"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate + 1).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate).toString()).split("\n"))
        }
    }

    GradientButton{
        id: minusX
        x: 230
        y: 40
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "-"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate - 1).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate).toString()).split("\n"))
        }
    }

    GradientText{
        id: textY
        x: 20
        y: 100
        width: 140
        height: 50
        text: "Move by Y"
        fontName: montserratFont.name
    }

    GradientButton{
        id: plusY
        x: 170
        y: 100
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "+"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate + 1).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate).toString()).split("\n"))
        }
    }

    GradientButton{
        id: minusY
        x: 230
        y: 100
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "-"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate - 1).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate).toString()).split("\n"))
        }
    }

    GradientText{
        id: textZ
        x: 20
        y: 160
        width: 140
        height: 50
        text: "Move by Z"
        fontName: montserratFont.name
    }

    GradientButton{
        id: plusZ
        x: 170
        y: 160
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "+"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate + 1).toString()).split("\n"))
        }
    }

    GradientButton{
        id: minusZ
        x: 230
        y: 160
        width: 50
        height: 50
        colorButton: "#539ef3"
        text: "-"
        radius: 10
        fontText: 25
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01 X" + Number(MillingMachine.xCoordinate).toString() +
                                         "Y" + Number(MillingMachine.yCoordinate).toString() +
                                         "Z" + Number(MillingMachine.zCoordinate - 1).toString()).split("\n"))
        }
    }

    GradientTextEdit{
        id: textMove
        x: 20
        y: 220
        width: 140
        height: 50
        text: "X00Y00Z00"
        fontName: montserratFont.name
    }

    GradientButton{
        id: move
        x: 170
        y: 220
        width: 110
        height: 50
        colorButton: "#539ef3"
        text: "Move"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.startMilling(("N01 G01" + textMove.text).split("\n"))
            //console.log(textMove.text)
        }
    }

    GradientTextEdit{
        id: textSpeed
        x: 20
        y: 280
        width: 140
        height: 50
        text: "V4.00"
        fontName: montserratFont.name
    }

    GradientButton{
        id: speed
        x: 170
        y: 280
        width: 110
        height: 50
        colorButton: "#539ef3"
        text: "Velocity"
        radius: 10
        fontName: montserratFont.name
        onClicked:{
            MillingMachine.changeVelocity(parseFloat(textSpeed.text.substring(1)));
        }
    }
}
