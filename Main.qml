import Qt.labs.folderlistmodel
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.Particles3D
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
    width: 1440
    height: 900
    visible: true
    title: "Celestial Position Calculator"
    color: "#848895"

    PlanetModel {
        id: coordinates_model
    }

    Node {
        id: main_scene

        PointLight {
            id: point_light
            position: Qt.vector3d(0, 0, 0)
            brightness: 1
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

        /*component StarDelegate : Node {
            required property double x
            required property double y
            required property double z
            position: Qt.vector3d(x, y, z)

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                width: 1
                height: 1
            }
        }
        */

        Repeater3D {
            model: coordinates_model
            delegate: PlanetDelegate {}
        }

        CustomMaterial {
            id: star_material

            vertexShader: "qrc:/shaders/star.vert"
            fragmentShader: "qrc:/shaders/star.frag"
        }

        Model {
            id: instanced_star_quad
            property real size: 1
            scale: Qt.vector3d(size/100, size/100, size/100)
            source: "#Rectangle"
            instancing: StarInstanceTable {

            }

            materials: [star_material]
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
        color: "black"
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
            width: parent.width * 0.2
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
                    titleFont.bold: true
                    titleFont.pointSize: 13.0
                }

                Text {
                    id: animation_speed_title
                    text: "Animation speed"
                    font.bold: true
                    font.pointSize: 13.0
                }

                Slider {
                    id: animation_speed_slider
                    value: 0.3
                    from: 0.05
                    to: 2.0
                    width: parent.width * 0.9
                    //stepSize: 0.5
                    //snapMode: Slider.SnapOnRelease

                    onMoved: coordinates_model.set_animation_speed(animation_speed_slider.value)
                }

                Button {
                    text: "Start animation"
                    checkable: true
                    onToggled: coordinates_model.calculatePositionsRepeatedly()
                }

                RadioButton {
                    text: "hello"
                }
            }
        }
    }
}

