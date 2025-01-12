import QtQuick
import QtQuick3D

Item {
    property int distance: 1000
    property int durationAnimation: 2000

    ParallelAnimation{
        id: animationToIsometry
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: -45; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 45; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: 1.6*distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    ParallelAnimation{
        id: animationToTop
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: -90; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 90; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: distance*0.05; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance*0.025; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    ParallelAnimation{
        id: animationToRight
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 90; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: distance*0.2; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance*0.025; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    Node {
        id: standAloneScene

        DirectionalLight {
            ambientColor: Qt.rgba(1.5, 1.5, 1.5, 1.5)
        }

        Model {
            id: body
            source: "qrc:/models/MachineFrame.mesh"
            position: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(1.3, 1.3, 1.3)
            eulerRotation.x: -90
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.144, 0.144, 0.144, 0.7)
                }
            ]
        }

        OrthographicCamera {
            id: cameraView
            position: Qt.vector3d(distance, 1.6*distance, distance)
            eulerRotation.x: -45
            eulerRotation.y: 45
            eulerRotation.z: 0
        }
    }

    GradientRectangle{
        width: 800
        height: 765

        GradientRectangle{
            id: rectView3d
            x: 20
            y: 20
            width: 760
            height: 655
            colorCenter: "#8c8c8ccc"
            View3D {
                id: view3D
                anchors.fill: parent
                importScene: standAloneScene
                camera: cameraView
            }
        }

        GradientButton{
            anchors.left: rectView3d.left
            anchors.leftMargin: 30
            y: 695
            text: "Isometry view"
            width: 200
            height: 50
            colorButton: "#8c8c8ccc"
            onClicked: {
                animationToIsometry.restart()
            }
        }
        GradientButton{
            y: 695
            text: "Top view"
            width: 200
            height: 50
            colorButton: "#8c8c8ccc"
            anchors.horizontalCenter: rectView3d.horizontalCenter
            onClicked: {
                animationToTop.restart()
            }
        }
        GradientButton{
            anchors.right: rectView3d.right
            anchors.rightMargin: 30
            y: 695
            text: "Right view"
            width: 200
            height: 50
            colorButton: "#8c8c8ccc"
            onClicked: {
                animationToRight.restart()
            }
        }
    }
}
