import QtQuick 2.0
import QtQuick.Controls 2.12

Button {
    id: control

    width: 100
    anchors {
        top: parent.top
        bottom: parent.bottom
    }

    text: "Control"

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down ? "#1f7da6" : "#1f7da6"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        anchors.fill: parent
        border.color: "#8adbff"
        gradient: Gradient {
            GradientStop {
                id: backgroundTopColor
                position: 0.00
                color: "#fdfbfb"
            }
            GradientStop {
                id: backgroundBottomColor
                position: 1.00
                color: "#d1f0ff"
            }
        }
    }

    states: [
        State {
            name: "normal"
            when: !control.hovered
            PropertyChanges {
                target: backgroundTopColor
                color: "#fdfbfb"
            }
            PropertyChanges {
                target: backgroundBottomColor
                color: "#e8f8ff"
            }
        },
        State {
            name: "hovered"
            when: control.hovered && !control.down
            PropertyChanges {
                target: backgroundTopColor
                color: "#fdfbfb"
            }
            PropertyChanges {
                target: backgroundBottomColor
                color: "#baeaff"
            }
        },
        State {
            name: "clicked"
            when: control.down
            PropertyChanges {
                target: backgroundTopColor
                color: "#baeaff"
            }
            PropertyChanges {
                target: backgroundBottomColor
                color: "#ebedee"
            }
        }
    ]
}
