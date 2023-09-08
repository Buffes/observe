import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick3D
import Qt3D.Core
import observe

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: "Hello World"
    color: "#848895"

    PositionCalculator {
        id: calculator
    }

    NodeInstantiator {

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

        Model {
            source: "#Sphere"
            materials: [
                PrincipledMaterial {
                    baseColor: "#BFBFBF"
                    metalness: 0.8
                    roughness: 0.3
                    opacity: 1.0
                }

            ]
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
                    onMoved: {
                        three_values.value1 = value
                        value_text.text = three_values.value1
                    }
                }

            Text {
                id: value_text
                text: "0.0"
            }

            Button {
                text: "Submit"
                onPressed: calculator.calculatePosition(1990, 4, 19, 0, 0, 0)
            }
        }
    }
}

