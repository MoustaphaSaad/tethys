import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import UI 1.0
import QtGraphicalEffects 1.0

Window {
    id: window
    visible: true
    width: 1280
    height: 720
    title: qsTr("zdbg")

    readonly property FontLoader logoFont: FontLoader {
        source: "qrc:/fonts/MajorMonoDisplay-Regular.ttf"
    }
    readonly property FontLoader monoFont: FontLoader {
        source: "qrc:/fonts/RobotoMono-Regular.ttf"
    }

    Rectangle {
        id: topBarBackground
        height: 50
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#fdfbfb"
            }

            GradientStop {
                position: 1
                color: "#ebedee"
            }
        }

        Text {
            id: logo
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            text: qsTr("Tethys Debugger")
            leftPadding: 5
            font.family: "Major Mono Display"
            font.pointSize: 16
        }

        Row {
            layoutDirection: Qt.RightToLeft
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: logo.right
                right: parent.right
                leftMargin: 10
            }

            TopBarButton {
                width: 150
                text: "Load Package"
                font.family: "Roboto Mono"
                font.pointSize: 10
            }
        }
    }

    DropShadow {
        id: topBarShadow
        anchors.fill: topBarBackground
        source: topBarBackground
        verticalOffset: 0
        radius: 8.0
        samples: 17
        color: "#54294274"
    }

    Shortcut {
        sequence: "F5"
        onActivated: {
            window.close()
            driver.loadQml()
        }
    }
}
