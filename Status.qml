import MillingMachine

GradientRectangle{
    width: 300
    height: 100

    GradientRectangle{
        id: ledCOMport
        x: 10
        y: 20
        width: 20
        height: 20
        radius: 10
        colorCenter: MillingMachine.openedPort ? "lightgreen" : "red"
    }

    GradientText{
        x: 40
        y: 20
        width: 90
        height: 20
        text: "COM-port"
    }

    GradientRectangle{
        id: ledProcessing
        x: 140
        y: 20
        width: 20
        height: 20
        radius: 10
        colorCenter: MillingMachine.processing ? "lightgreen" : "red"
    }

    GradientText{
        x: 175
        y: 20
        width: 90
        height: 20
        text: "Processing"
    }

    GradientRectangle{
        x: 10
        y: 60
        width: 20
        height: 20
        radius: 10
        colorCenter: MillingMachine.calibrated ? "lightgreen" : "red"
    }

    GradientText{
        id: ledPartZero
        x: 40
        y: 60
        width: 90
        height: 20
        text: "Part Zero"
    }

    GradientRectangle{
        x: 140
        y: 60
        width: 20
        height: 20
        radius: 10
        colorCenter: "red"
    }

    GradientText{
        x: 185
        y: 60
        width: 90
        height: 20
        text: "Vaccum Clear"
    }
}
