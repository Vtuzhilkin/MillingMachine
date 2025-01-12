import QtQuick
import QtQuick.Effects

Item{
    property color colorStart: "white"
    property color colorCenter: "grey"
    property color colorEnd: "black"
    property int radius: 20
    property int shadowHorizontalOffset: 5
    property int shadowVerticalOffset: 5

    Rectangle{
        id: rectangle
        anchors.fill: parent
        radius: parent.radius
        color: colorCenter
    }

    ShaderEffectSource{
        id: rectangleSource
        sourceItem: rectangle
        anchors.fill: rectangle
        hideSource: true
        visible: false
    }

    MultiEffect {
        id: rectangleMulti
        anchors.fill: rectangleSource
        source: rectangleSource
        shadowBlur: 0.8
        shadowHorizontalOffset: parent.shadowHorizontalOffset
        shadowVerticalOffset: parent.shadowVerticalOffset
        shadowEnabled: true
    }

    ShaderEffect{
        id: rectangleShader
        anchors.fill: rectangleSource
        property variant source: rectangleSource
        property int coefGradient: 20
        property color colorStart: parent.colorStart
        property color colorCenter: parent.colorCenter
        property color colorEnd:  parent.colorEnd
        fragmentShader: "qrc:/shaders/gradientShadow.frag.qsb"
    }
}

