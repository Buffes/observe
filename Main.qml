import Qt.labs.folderlistmodel
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import observe

/*
Todo list
- controls for heliocentric/geocentric
- controls for animation speed and resolution
- different sizes for the planets
- astronomical map view (RA/declination)
- make the sun into the scene's light source
- load star positions from here http://tdc-www.harvard.edu/catalogs/bsc5.html and generate background with stars
- textures for the planets
*/

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

        component PlanetDelegate : Node {
            required property double x
            required property double y
            required property double z
            required property color p_color;
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
                            diffuseColor: p_color
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
            position: Qt.vector3d(0, 0, 0)
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
            /*environment: SceneEnvironment {
                backgroundMode: SceneEnvironment.SkyBox
                lightProbe: Texture {
                    source: "qrc:/img/skybox.ktx"
                }
            }*/
        }

        WasdController {
            controlledObject: camera
            speed: 0.1
        }

        Rectangle {
            id: gui_background
            width: parent.width * 0.25
            height: parent.height
            color: mouse.hovered ? "#a9a9a9" : "tomato"
            anchors.right: parent.right

            HoverHandler {
                id: mouse
            }

            Column {
                id: controls
                width: parent.width
                spacing: 10
                padding: 10

                MyCalendar {
                    id: calendar
                    onDateSelected: date => coordinates_model.calculatePositions(date)
                }

                Button {
                    text: "Start animation"
                    checkable: true
                    onToggled: coordinates_model.calculatePositionsRepeatedly()
                }

                Slider {
                    id: animation_speed_slider
                    value: 1
                    from: 0.5
                    to: 5
                    //stepSize: 0.5
                    //snapMode: Slider.SnapOnRelease

                    signal valueChanged(float value)

                    Component.onCompleted: animation_speed_slider.valueChanged.connect(coordinates_model.animation_speed_changed)

                    onMoved: animation_speed_slider.valueChanged(value)
                }

                RadioButton {
                    text: "hello"
                }
            }
        }
    }
}

