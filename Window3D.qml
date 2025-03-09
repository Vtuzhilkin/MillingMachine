import QtQuick
import QtQuick3D
import MillingMachine

Item {
    id: window3D
    property int distance: 1000
    property int durationAnimation: 2000
    property double scaleModel: 1.1

    ParallelAnimation{
        id: animationToIsometry
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: -45; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 45; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 125; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: distance*0.8; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: distance*1.2; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: window3D; property: "scaleModel"; to: 1.1; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    ParallelAnimation{
        id: animationToTop
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 90; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: window3D; property: "scaleModel"; to: 1.1; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    ParallelAnimation{
        id: animationToRight
        NumberAnimation {
            target: cameraView; property: "eulerRotation.x"; to: -90; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.y"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "eulerRotation.z"; to: 180; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "x"; to: 0; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "y"; to: distance; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cameraView; property: "z"; to: distance*0.2; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: window3D; property: "scaleModel"; to: 1.1; duration: durationAnimation; easing.type: Easing.InOutQuad
        }
    }

    Node {
        id: standAloneScene

        DirectionalLight {
            ambientColor: Qt.rgba(1.5, 1.5, 1.5, 1.5)
        }

        Model {
            id: body
            source: "qrc:/models/Body.mesh"
            position: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.144, 0.144, 0.144, 0.7)
                }
            ]
        }

        Model {
            id: corners
            source: "qrc:/models/Corners.mesh"
            position: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#116062"
                }
            ]
        }

        Model {
            id: tableGuides
            source: "qrc:/models/TableGuides.mesh"
            position: Qt.vector3d(0, 0, 30)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#ff8c00"
                }
            ]
        }

        Model {
            id: table
            source: "qrc:/models/Table.mesh"
            z: 220 - MillingMachine.zCoordinate
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.144, 0.144, 0.144, 0.7)
                }
            ]
        }

        Model {
            id: tableStepper
            source: "qrc:/models/TableSteppers.mesh"
            position: Qt.vector3d(0, 0, 30)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#303030"
                }
            ]
        }

        Model {
            id: guides
            source: "qrc:/models/Guides.mesh"
            position: Qt.vector3d(0, 0, 275)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#ff8c00"
                }
            ]
        }

        Model {
            id: guidesStepper
            source: "qrc:/models/GuidesStepper.mesh"
            position: Qt.vector3d(0, 0, 275)
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#303030"
                }
            ]
        }

        Model {
            id: guidesHolders
            source: "qrc:/models/GuidesHolders.mesh"
            x: 95 - MillingMachine.xCoordinate
            y: 0
            z: 275
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#116062"
                }
            ]
        }

        Model {
            id: guidesLSteppers
            source: "qrc:/models/GuidesLStepper.mesh"
            x: 95 - MillingMachine.xCoordinate
            y: 0
            z: 275
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#303030"
                }
            ]
        }

        Model {
            id: spindelHolder
            source: "qrc:/models/SpindelHolder.mesh"
            x: 95 - MillingMachine.xCoordinate
            y: 95 - MillingMachine.yCoordinate
            z: 275
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#116062"
                }
            ]
        }

        Model {
            id: spindel
            source: "qrc:/models/Spindel.mesh"
            x: 95 - MillingMachine.xCoordinate
            y: 95 - MillingMachine.yCoordinate
            z: 275
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#800000"
                }
            ]
        }

        Model {
            id: guidesS
            source: "qrc:/models/GuidesS.mesh"
            x: 95 - MillingMachine.xCoordinate
            z: 275
            scale: Qt.vector3d(scaleModel, scaleModel, scaleModel)
            materials: [
                DefaultMaterial {
                    diffuseColor: "#ff8c00"
                }
            ]
        }

        OrthographicCamera {
            id: cameraView
            position: Qt.vector3d(0.8*distance, 1.2*distance, distance)
            eulerRotation.x: -45
            eulerRotation.y: 45
            eulerRotation.z: 125
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
