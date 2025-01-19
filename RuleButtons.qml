import MillingMachine

GradientRectangle{
    width: 300
    height: 130

    GradientButton{
        id: startButton
        text: "Start"
        x: 10
        y: 10
        width: parent.width/2 - 15
        height: 50
        colorButton: "#539ef3"
        radius: 10
        onClicked:{
            MillingMachine.startMilling(listCommands.text.split('\n'));
        }
    }
    GradientButton{
        id: stopButton
        text: "Stop"
        x: parent.width/2 + 5
        y: 10
        width: parent.width/2 - 15
        height: 50
        colorButton: "#539ef3"
        radius: 10
    }
    GradientButton{
        id: startPause
        text: "Pause"
        x: 10
        y: startButton.y + startButton.height + 10
        width: parent.width/2 - 15
        height: 50
        colorButton: "#539ef3"
        radius: 10
    }
    GradientButton{
        id: startContinue
        text: "Continue"
        x: parent.width/2 + 5
        y: stopButton.y + stopButton.height + 10
        width: parent.width/2 - 15
        height: 50
        colorButton: "#539ef3"
        radius: 10
    }
}
