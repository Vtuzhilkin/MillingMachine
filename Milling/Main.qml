import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtQuick3D


Window {
    id: window
    visible: true
    title: qsTr("Milling")
    visibility: "Maximized"
    property int distance: 2000
    property int k: 1
    property int coef_time: 250

    property double next_crd_x: 0
    property double next_crd_y: 0
    property double next_crd_z: 0

    property double relative_x: 0
    property double relative_y: 0
    property double relative_z: 0

    property int i: 0
    property int number_lines: 0
    property var coordinates: []

    Connections{
        target: millingMachine
        onMySignal:{
            startMoving.start()
        }
    }

    SequentialAnimation{
        id: startMoving
        running: false
        ScriptAction{
            script: {
                if(i < number_lines){
                    if(millingMachine.getFlagEmptyCoordinates()){
                        millingMachine.readCommandString(coordinates[i]);
                        ++i;
                    }
                    millingMachine.getNextCoordinate();
                    next_crd_x = millingMachine.getCoordinateX() + relative_x;
                    next_crd_y = millingMachine.getCoordinateY() + relative_y;
                    next_crd_z = millingMachine.getCoordinateZ() + relative_z;
                    move.start()
                } else{
                    millingMachine.change_moving_coordinates(0)
                }
            }
        }
    }

    ParallelAnimation{
        id: move
        NumberAnimation {
            id: movingX
            target: spindel
            property: "x"
            duration: Math.abs(next_crd_x - spindel.x)*coef_time
            to: next_crd_x
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            id: movingY
            target: spindelWithGuides
            property: "z"
            duration: Math.abs(next_crd_y - spindelWithGuides.z)*coef_time
            to: next_crd_y
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            id: movingZ
            target: table
            property: "y"
            duration: Math.abs(next_crd_z - table.y)*coef_time
            to: next_crd_z
            easing.type: Easing.InOutQuad
        }
    }

    Node {
        id: standAloneScene

        DirectionalLight {
            ambientColor: Qt.rgba(1.0, 1.0, 1.0, 1.0)
        }

        Model {
            id: body
            source: "resources/MachineFrame.mesh"
            position: Qt.vector3d(100, -210, 70)
            scale: Qt.vector3d(1, 1, 1)
            eulerRotation.x: -90
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.144, 0.144, 0.144, 0.7)
                }
            ]
        }
        Node{
            id: spindelWithGuides
            Model {
                id: yGuides
                source: "resources/YGuides.mesh"
                position: Qt.vector3d(109, 43, 0)
                eulerRotation.y: 180
                eulerRotation.x: -90
                scale: Qt.vector3d(1, 1, 1)
                materials: [
                    DefaultMaterial {
                        diffuseColor: Qt.rgba(0.255, 0.128, 0, 0.7)
                    }
                ]
            }
            Model {
                id: spindel
                source: "resources/Spindel.mesh"
                position: Qt.vector3d(0, 43, 0)
                eulerRotation.y: 180
                eulerRotation.x: -90
                scale: Qt.vector3d(1, 1, 1)
                materials: [
                    DefaultMaterial {
                        diffuseColor: Qt.rgba(0.204, 0, 0, 0.7)
                    }
                ]
            }
        }
        Model {
            id: table
            source: "resources/Table.mesh"
            position: Qt.vector3d(96, 0, 71.4)
            eulerRotation.x: -90
            scale: Qt.vector3d(1, 1, 1)
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.1, 0.1, 0.1, 0.4)
                }
            ]
        }

        OrthographicCamera {
            id: cameraPerspectiveIsometry
            position: Qt.vector3d(distance*1.02, distance*1.35, distance)
            eulerRotation.x: -45
            eulerRotation.y: 45
        }

        OrthographicCamera {
            id: cameraPerspectiveTop
            y: distance
            x: distance*0.05
            z: distance*0.025
            eulerRotation.x: -90
            eulerRotation.z: 90
        }

        OrthographicCamera {
            id: cameraPerspectiveRight
            x: distance
            y: -distance*0.02
            z: distance*0.025
            eulerRotation.y: 90
        }
    }
    Rectangle {
        id: isometry
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width / 3
        height: parent.height
        color: "#8c8c8ccc"
        border.color: "black"

        View3D {
            id: topLeftView
            anchors.fill: parent
            importScene: standAloneScene
            camera: cameraPerspectiveIsometry

        }

        Label {
            text: "Изометрия"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            color: "#222840"
            font.pointSize: 14
        }
    }
    Rectangle {
        id: top
        anchors.top: parent.top
        anchors.left: isometry.right
        width: parent.width / 3
        height: parent.height / 2
        color: "#8c8c8ccc"
        border.color: "black"

        View3D {
            id: bottomLeftView
            anchors.fill: parent
            importScene: standAloneScene
            camera: cameraPerspectiveTop
        }

        Label {
            text: "Сверху"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            color: "#222840"
            font.pointSize: 14
        }
    }

    Rectangle {
        id: right
        anchors.bottom: parent.bottom
        anchors.left: isometry.right
        width: parent.width / 3
        height: parent.height / 2
        color: "#8c8c8ccc"
        border.color: "black"

        View3D {
            id: bottomRightView
            anchors.fill: parent
            importScene: standAloneScene
            camera: cameraPerspectiveRight
        }

        Label {
            text: "Справа"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            color: "#222840"
            font.pointSize: 14
        }
    }
    Rectangle {
        id: columnButtons
        width: parent.width/ 3
        height: parent.height / 3
        anchors.right: parent.right
        anchors.top: parent.top
        color: "#696969"
        Rectangle{
            id: currentCoordinateText
            width: parent.width*4/8
            height: parent.height/6
            anchors.top: parent.top
            anchors.left: parent.left
            radius: 10
            color: "#20B2AA"
            border.width: 1
            Text{
                text: "Текущая координата"
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        Rectangle{
            id: currentCoordinate
            anchors.top: currentCoordinateText.top
            anchors.left: currentCoordinateText.right
            width: parent.width*3/8
            height: parent.height/6
            radius: 10
            color: "#20B2AA"
            border.width: 1
            Text{
                text: "X" + Math.round(spindel.x,3)
                      + " Y" + Math.round(spindelWithGuides.z,3)
                      + " Z" + Math.round(-table.y,3)
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }


        Button{
            id: goToZero
            text: "Ноль"
            width: parent.width*1/8
            height: parent.height/6
            anchors.top: currentCoordinateText.top
            anchors.left: currentCoordinate.right
            font.pixelSize: height / 2
            onClicked:{
                next_crd_x = 0
                next_crd_y = 0
                next_crd_z = 0
                relative_x = 0
                relative_y = 0
                relative_z = 0
                millingMachine.readCoordinates("X0Y0Z0")
                millingMachine.sendCoordinate()
                move.start()
            }

            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Rectangle{
            id: relativeCoordinateText
            width: parent.width*4/8
            height: parent.height/6
            anchors.top: currentCoordinate.bottom
            anchors.left: parent.left
            color: "#20B2AA"
            border.width: 1
            radius: 10
            Text{
                width: parent.width
                height: parent.height
                text: "Относит. координаты"
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle{
            id: relativeCoordinate
            width: parent.width*3/8
            height: parent.height/6
            anchors.top: relativeCoordinateText.top
            anchors.left: relativeCoordinateText.right
            color: "#20B2AA"
            border.width: 1
            radius: 10
            Text{
                width: parent.width
                height: parent.height
                text: "X" + Math.round(spindel.x-relative_x,3)
                      + " Y" + Math.round(spindelWithGuides.z-relative_y,3)
                      + " Z" + Math.round(-(table.y-relative_z),3)
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

        }

        Button{
            id: setRelativeCoordinates
            text: "Ноль"
            width: parent.width*1/8
            height: parent.height/6
            anchors.top: relativeCoordinate.top
            anchors.left: relativeCoordinate.right
            font.pixelSize: height / 2
            onClicked:{
                relative_x = spindel.x
                relative_y = spindelWithGuides.z
                relative_z = table.y
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Rectangle{
            id: textmoveX
            width: parent.width*3/4
            height: parent.height/6
            anchors.top: relativeCoordinateText.bottom
            anchors.left: parent.left
            color: "#20B2AA"
            border.width: 1
            radius: 10
            Text{
                text: "Переместить по X"
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

        }

        Button {
            id: posmoveX
            width: parent.width/8
            height: textmoveX.height
            anchors.top: textmoveX.top
            anchors.left: textmoveX.right
            text: "+"
            font.pixelSize: height
            onClicked: {
                if(spindel.x < 150 && millingMachine.nextCommand()){
                    next_crd_x += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: negmoveX
            width: parent.width/8
            height: textmoveX.height
            anchors.bottom: textmoveX.bottom
            anchors.left: posmoveX.right
            text: "-"
            font.pixelSize: height
            onClicked: {
               if(spindel.x > 0 && millingMachine.nextCommand()){
                   next_crd_x -= k
                   millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                   millingMachine.sendCoordinate()
                   move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }

        Rectangle{
            id: textmoveY
            width: parent.width*3/4
            height: parent.height/6
            anchors.top: textmoveX.bottom
            anchors.left: parent.left
            color: "#20B2AA"
            border.width : 1
            radius: 10
            Text{
                text: "Переместить по Y"
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }



        Button {
            id: posmoveY
            width: parent.width/8
            height: textmoveY.height
            anchors.top: textmoveY.top
            anchors.left: textmoveY.right
            text: "+"
            font.pixelSize: height
            onClicked: {
                if(spindelWithGuides.z < 180 && millingMachine.nextCommand()){
                    next_crd_y += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: negmoveY
            width: parent.width/8
            height: textmoveY.height
            anchors.bottom: textmoveY.bottom
            anchors.left: posmoveY.right
            text: "-"
            font.pixelSize: height
            onClicked: {
                if(spindelWithGuides.z > 0 && millingMachine.nextCommand()){
                    next_crd_y -= k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Rectangle{
            id: textmoveZ
            width: parent.width*3/4
            height: parent.height/6
            anchors.top: textmoveY.bottom
            anchors.left: parent.left
            color: "#20B2AA"
            border.width: 1
            radius: 10
            Text{
                text: "Переместить по Z"
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

        }
        Button {
            id: posmoveZ
            width: parent.width/8
            height: textmoveZ.height
            anchors.top: textmoveZ.top
            anchors.left: textmoveZ.right
            text: "+"
            font.pixelSize: height
            onClicked: {
                if(table.y > -160 && millingMachine.nextCommand()){
                    next_crd_z -= k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + -next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: negmoveZ
            width: parent.width/8
            height: textmoveZ.height
            anchors.bottom: textmoveZ.bottom
            anchors.left: posmoveZ.right
            text: "-"
            font.pixelSize: height
            onClicked: {
                if(table.y < 0 && millingMachine.nextCommand()){
                    next_crd_z += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + -next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Rectangle{
            id: textmove
            width: parent.width / 2
            height: parent.height/6
            anchors.top: textmoveZ.bottom
            anchors.left: textmoveZ.left
            color: "#20B2AA"
            border.width: 1
            radius: 10
            TextInput{
                id: movetext
                text: "X00Y00Z00"
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Button {
            id: movemachine
            width: parent.width/2
            height: parent.height/6
            anchors.top: textmove.top
            anchors.left: textmove.right
            text: "Переместить"
            font.pixelSize: height / 2
            onClicked: {
                if(millingMachine.nextCommand()){
                    millingMachine.readCoordinates(movetext.text);
                    next_crd_x = millingMachine.getCoordinateX();
                    next_crd_y = millingMachine.getCoordinateY();
                    next_crd_z = millingMachine.getCoordinateZ();
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
            background: Rectangle {
                color: movemachine.pressed ? "#FF4500": movemachine.hovered ? "#FF8C00": "#FFA500"
                width: parent.width
                height: parent.height
                radius: 10
                border.width: 1
            }
        }
    }
    ScrollView{
        id: scrollViewCoordinates
        anchors.top: columnButtons.bottom
        anchors.right: parent.right
        height: parent.height / 2
        width: parent.width / 3
        TextArea{
            id: list_coordinates
            font.pixelSize: movetext.font.pixelSize
        }
    }

    Rectangle{
        id: buttons
        anchors.top: scrollViewCoordinates.bottom
        anchors.right: parent.right
        width: parent.width / 3
        height: parent.height / 6
        color: "#8c8c8ccc"
        Button {
            id: start
            anchors.top: buttons.top
            anchors.left: buttons.left
            height: parent.height / 2
            width: parent.width / 3
            text: "Старт"
            font.pixelSize: height / 3
            onClicked: {
                i = 0
                coordinates = list_coordinates.text.split("\n")
                number_lines = list_coordinates.lineCount
                millingMachine.change_moving_coordinates(1)
                millingMachine.sendCoordinate()
            }

            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: stop
            anchors.top: parent.top
            anchors.left: start.right
            height: parent.height / 2
            width: parent.width / 3
            text: "Стоп"
            font.pixelSize: height / 3
            onClicked: {
                moving_list = false
                millingMachine.change_moving_coordinates(0)
            }

            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: continue_moving
            anchors.top: parent.top
            anchors.left: stop.right
            height: parent.height / 2
            width: parent.width / 3
            text: "Продолжить"
            font.pixelSize: height / 3
            onClicked: {
                moving_list = true
            }

            background: Rectangle {
                color: "#FFA500"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: open_com_port
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: parent.height / 2
            width: parent.width / 2

            text: "Открыть COM-port"
            font.pixelSize: height / 3
            onClicked: {
                millingMachine.openCOMport()
            }

            background: Rectangle {
                color: "#20B2AA"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
        Button {
            id: close_com_port
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            height: parent.height / 2
            width: parent.width / 2

            text: "Закрыть COM-port"
            font.pixelSize: height / 3
            onClicked: {
                millingMachine.closeCOMport()
            }

            background: Rectangle {
                color: "#20B2AA"
                radius: 10
                Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 10
                            color: "transparent"
                            border.width: 1
                        }
            }
        }
    }
}
