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
        text: "Position: X00Y00Z00"
        fontName: montserratFont.name
    }

    GradientText{
        id: textplusX
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
    }

    GradientText{
        id: textplusY
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
    }

    GradientText{
        id: textplusZ
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
    }

    GradientTextEdit{
        id: textMove
        x: 20
        y: 220
        width: 140
        height: 50
        text: "X000Y000Z000"
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
            console.log(textMove.text)
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
    }
}
