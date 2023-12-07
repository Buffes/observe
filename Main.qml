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
- rewrite planet repeater to use instancing like the star
- selection of objects and information showing up.
- texture for the stars
- controls for heliocentric/geocentric
- different sizes for the planets
- astronomical map view (RA/declination)
- make the sun into the scene's light source
- textures for the planets - 
*/

Window {
    id: window
    width: 1440
    height: 900
    visible: true
    title: "Celestial Position Calculator"
    color: "#848895"
    property var planetModel
    property var starInstanceTable
    property var selectionHandler

    /*PlanetModel {
        id: coordinates_model
    }*/

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
                scale: Qt.vector3d(0.1, 0.1, 0.1)

                materials: [ DefaultMaterial {
                            diffuseColor: p_color
                        }
                    ]
            }
        }

        Repeater3D {
            model: window.planetModel
            delegate: PlanetDelegate {}
        }

        CustomMaterial {
            id: star_material
            shadingMode: CustomMaterial.Unshaded
            vertexShader: "qrc:/shaders/star.vert"
            fragmentShader: "qrc:/shaders/star.frag"
        }

        Model {
            id: instanced_star_quad
            source: "#Rectangle"
            instancing: window.starInstanceTable
            castsShadows: false
            castsReflections: false

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
            id: main_view3d
            anchors.fill: parent
            importScene: main_scene
            camera: camera
            environment: SceneEnvironment {
                backgroundMode: SceneEnvironment.SkyBox
                lightProbe: Texture {
                    //source: "qrc:/hdr/skybox.ktx"
                    source: "qrc:/img/constellation_figures_4k.tif"
                }
            }
        }

        WasdController {
            controlledObject: camera
            speed: 0.1
        }

        MouseArea {
            id: main_mouse
            anchors.fill: main_view3d

            property real last_x
            property real last_y

            onWheel: event => {
                         let new_fov = camera.fieldOfView - event.angleDelta.y * 0.05;
                         camera.fieldOfView = Math.max(10.0, Math.min(150.0, new_fov));
                     }

            onClicked: event => {
                           let clickedPoint = main_view3d.mapTo3DScene(Qt.vector3d(event.x, event.y, 10.0));
                           main_scene.selectionHandler.rayPick(camera.position, clickedPoint);
                       }
        }

        Rectangle {
            id: gui_background
            width: parent.width * 0.2
            height: parent.height
            color: "#FFa9a9a9"
            opacity: 0.25
            anchors.right: parent.right

            HoverHandler {
                id: controls_mouse
            }

            states: State {
                name: "hovered"; when: controls_mouse.hovered
                PropertyChanges {
                    target: gui_background
                    opacity: 1.0
                }
            }

            transitions: Transition {
                NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 150 }
            }

            Column {
                id: controls
                width: parent.width
                spacing: 10
                padding: 10

                MyCalendar {
                    id: calendar
                    anchors.horizontalCenter: parent.horizontalCenter
                    onDateSelected: date => window.planetModel.calculatePositions(date)
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

                    onMoved: window.planetModel.setAnimationSpeed(animation_speed_slider.value)
                }

                Button {
                    text: "Start animation"
                    checkable: true
                    onToggled: window.planetModel.calculatePositionsRepeatedly()
                }

                RadioButton {
                    text: "hello"
                }
            }
        }
    }
}

