import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import observe

Window {
    id: window
    width: 960
    height: 600
    visible: true
    title: "Hello World"
    color: "#848895"

    Vector3DListModel {
        id: coordinates_model
    }

    Node {
        id: main_scene

        PointLight {
            id: point_light
            position: Qt.vector3d(-200, 200, 200)
            brightness: 20

            SequentialAnimation on x {
                loops: Animation.Infinite

                PropertyAnimation {
                    to: 200
                    duration: 3000
                }
                PropertyAnimation {
                    to: -200
                    duration: 3000
                }
            }
        }

        /*ListModel {
            id: coordinateModel

            ListElement {

                name: "saturn"
                x: 4.22006
                y: -9.0088
                z: 0.0323988
            }
            ListElement {

                name: "jupiter"
                x: -0.484747
                y: 5.49442
                z: 0.0100939
            }
            ListElement {

                name: "venus"
                x: 0.800765
                y: -0.240035
                z: -0.00529978
            }
            ListElement {

                name: "mercury"
                x: 0.513227
                y: 0.543184
                z: 0.0386991
            }
        }*/

        component PlanetDelegate : Node {
            required property double x
            required property double y
            required property double z
            position: Qt.vector3d(x, y, z)

            /*Text {
                text: qsTr(name)
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                font.pixelSize: 1
            }*/

            Model {
                source: "#Sphere"
                scale: Qt.vector3d(0.01, 0.01, 0.01)

                materials: [ DefaultMaterial {
                            diffuseColor: "blue"
                        }
                    ]
            }

        }

        Repeater3D {
            model: coordinates_model

            delegate: PlanetDelegate {}
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 0, 400)
            fieldOfView: 90
        }
    }

    Rectangle {
        id: main_area
        anchors.fill: parent
        color: "#cafebabe"
        View3D {
            anchors.fill: parent
            importScene: main_scene
        }

        WasdController {
            controlledObject: camera
        }

        Column {
            id: controls
            anchors.right: parent.right
            width: parent.width * 0.25
            spacing: 10
            padding: 10

            TextField {
                placeholderText: "Write here..."
            }

            Slider {
                id: slider
                from: 0.0
                to: 10.0
            }

            Text {
                id: value_text
                text: "0.0"
            }

            Button {
                text: "Submit"
                onPressed: coordinates_model.calculatePositions(1990, 1, 19, 0, 0, 0)
            }

            Calendar {
                width: parent.width
            }
        }
    }
}

