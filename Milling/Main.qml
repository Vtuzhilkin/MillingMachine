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
    minimumHeight: 380
    minimumWidth: 640
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
    property var coordinates: []
    property bool operating: false

    Connections{
        target: millingMachine
        function onStartProcessing(){ // Сигнал из C++ файла для старта обработки и анимации
            startMoving.start()
        }

        function onSignalUnlock(open){ // Сигнал из C++ для разблокировки/блокировки кнопок после открытия/закрытия COM-порта
            if(i === 0){
                start.enabled = open;
                enabledManualOperating(open);
            }else{
                continue_moving.enabled = open
            }
            open_com_port.enabled = !open
            close_com_port.enabled = open
        }
    }

    function enabledManualOperating(open_){
        goToZero.enabled = open_;
        setRelativeCoordinates.enabled = open_;
        posmoveX.enabled = open_
        negmoveX.enabled = open_
        posmoveY.enabled = open_
        negmoveY.enabled = open_
        posmoveZ.enabled = open_
        negmoveZ.enabled = open_
        movemachine.enabled = open_
    }

    SequentialAnimation{
        id: startMoving
        running: false
        ScriptAction{
            script: {
                if(operating){
                    if(i < list_coordinates.lineCount){
                        if(millingMachine.emptyListCoordinates()){
                            millingMachine.readCommandString(coordinates[i]);
                            ++i;
                        }
                        millingMachine.getNextCoordinate();
                        next_crd_x = millingMachine.getCoordinateX() + relative_x;
                        next_crd_y = millingMachine.getCoordinateY() + relative_y;
                        next_crd_z = millingMachine.getCoordinateZ() + relative_z;
                        move.restart()
                        millingMachine.sendCoordinate();
                        console.log(i, next_crd_x, " ", next_crd_y, " ", next_crd_z)
                    }else{
                        console.log(i)
                        i = 0;
                        list_coordinates.enabled = true
                        close_com_port.enabled = true
                        stop.enabled = false
                        start.enabled = true
                        enabledManualOperating(true)
                    }
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
                text: "X" + Number(spindel.x).toFixed(2)
                      + " Y" + Number(spindelWithGuides.z).toFixed(2)
                      + " Z" + Number(-table.y).toFixed(2)
                width: parent.width
                height: parent.height
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        ButtonTemplate {
            id: goToZero
            enabled: false
            width: parent.width*1/8
            height: parent.height/6
            anchors.top: currentCoordinateText.top
            anchors.left: currentCoordinate.right
            defaultFontSize: height / 2
            button_text: "Ноль"
            onButtonClicked: {
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
                text: "X" + Number(spindel.x-relative_x).toFixed(2)
                      + " Y" + Number(spindelWithGuides.z-relative_y).toFixed(2)
                      + " Z" + Number(-(table.y-relative_z)).toFixed(2)
                font.pixelSize: height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }        
        ButtonTemplate {
            id: setRelativeCoordinates
            enabled: false
            width: parent.width*1/8
            height: parent.height/6
            anchors.top: relativeCoordinate.top
            anchors.left: relativeCoordinate.right
            defaultFontSize: height / 2
            button_text: "Ноль"
            onButtonClicked: {
                if(spindel.x < 150 && millingMachine.nextCommand()){
                    relative_x = spindel.x
                    relative_y = spindelWithGuides.z
                    relative_z = table.y
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
        ButtonTemplate {
            id: posmoveX
            enabled: false
            width: parent.width/8
            height: textmoveX.height
            anchors.top: textmoveX.top
            anchors.left: textmoveX.right
            defaultFontSize: height
            button_text: "+"
            onButtonClicked: {
                if(spindel.x < 150 && millingMachine.nextCommand()){
                    next_crd_x += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
        }
        ButtonTemplate {
            id: negmoveX
            enabled: false
            width: parent.width/8
            height: textmoveX.height
            anchors.bottom: textmoveX.bottom
            anchors.left: posmoveX.right
            defaultFontSize: height
            button_text: "-"
            onButtonClicked: {
                if(spindel.x > 0 && millingMachine.nextCommand()){
                    next_crd_x -= k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
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
        ButtonTemplate {
            id: posmoveY
            enabled: false
            width: parent.width/8
            height: textmoveY.height
            anchors.top: textmoveY.top
            anchors.left: textmoveY.right
            defaultFontSize: height
            button_text: "+"
            onButtonClicked: {
                if(spindelWithGuides.z < 180 && millingMachine.nextCommand()){
                    next_crd_y += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
        }
        ButtonTemplate {
            id: negmoveY
            enabled: false
            width: parent.width/8
            height: textmoveY.height
            anchors.bottom: textmoveY.bottom
            anchors.left: posmoveY.right
            defaultFontSize: height
            button_text: "-"
            onButtonClicked: {
                if(spindelWithGuides.z > 0 && millingMachine.nextCommand()){
                    next_crd_y -= k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
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
        ButtonTemplate {
            id: posmoveZ
            enabled: false
            width: parent.width/8
            height: textmoveZ.height
            anchors.top: textmoveZ.top
            anchors.left: textmoveZ.right
            defaultFontSize: height
            button_text: "+"
            onButtonClicked: {
                if(table.y > -160 && millingMachine.nextCommand()){
                    next_crd_z -= k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + -next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
                }
            }
        }
        ButtonTemplate {
            id: negmoveZ
            enabled: false
            width: parent.width/8
            height: textmoveZ.height
            anchors.bottom: textmoveZ.bottom
            anchors.left: posmoveZ.right
            defaultFontSize: height
            button_text: "-"
            onButtonClicked: {
                if(table.y < 0 && millingMachine.nextCommand()){
                    next_crd_z += k
                    millingMachine.readCoordinates("X" + next_crd_x + "Y" + next_crd_y + "Z" + -next_crd_z)
                    millingMachine.sendCoordinate()
                    move.start()
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
        ButtonTemplate {
            id: movemachine
            enabled: false
            width: parent.width/2
            height: parent.height/6
            anchors.top: textmove.top
            anchors.left: textmove.right
            defaultFontSize: height / 2
            button_text: "Переместить"
            onButtonClicked: {
                if(millingMachine.nextCommand()){
                    millingMachine.readCoordinates(movetext.text);
                    next_crd_x = millingMachine.getCoordinateX();
                    next_crd_y = millingMachine.getCoordinateY();
                    next_crd_z = millingMachine.getCoordinateZ();
                    millingMachine.sendCoordinate()
                    move.start()
                }
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
        ButtonTemplate {
            id: start
            enabled: false
            anchors.top: buttons.top
            anchors.left: buttons.left
            height: parent.height / 2
            width: parent.width / 3
            button_text: "Старт"
            onButtonClicked: {
                if(list_coordinates.text !== ""){
                    console.log(list_coordinates.lineCount)
                    operating = true
                    start.enabled = false
                    stop.enabled = true
                    open_com_port.enabled = false
                    close_com_port.enabled = false
                    continue_moving.enabled = false
                    list_coordinates.enabled = false
                    enabledManualOperating(false)

                    i = 0
                    coordinates = list_coordinates.text.split("\n")
                    startMoving.start()
                }
            }
        }
        ButtonTemplate {
            id: stop
            enabled: false
            anchors.top: parent.top
            anchors.left: start.right
            height: parent.height / 2
            width: parent.width / 3
            button_text: "Стоп"
            onButtonClicked: {
                operating = false
                stop.enabled = false
                close_com_port.enabled = true
                continue_moving.enabled = true
            }
        }
        ButtonTemplate {
            id: continue_moving
            enabled: false
            anchors.top: parent.top
            anchors.left: stop.right
            height: parent.height / 2
            width: parent.width / 3
            button_text: "Продолжить"
            onButtonClicked: {
                operating = true
                stop.enabled = true
                continue_moving.enabled = false
                close_com_port.enabled = false
                startMoving.start()
            }
        }
        ButtonTemplate {
            id: open_com_port
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: parent.height / 2
            width: parent.width / 2
            defaultColor: "#20B2AA"
            pressColor: "#18867F"
            enterColor: "#48D2CC"
            button_text: "Открыть COM-port"
            onButtonClicked: {
                millingMachine.openCOMport()
            }
        }
        ButtonTemplate {
            id: close_com_port
            enabled: false
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            height: parent.height / 2
            width: parent.width / 2
            defaultColor: "#20B2AA"
            pressColor: "#18867F"
            enterColor: "#48D2CC"
            button_text: "Закрыть COM-port"
            onButtonClicked: {
                millingMachine.closeCOMport()
            }
        }
    }
}
