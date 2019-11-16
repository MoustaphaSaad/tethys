pragma Singleton
import QtQuick 2.12

QtObject {
//    property FontLoader fontLoader: FontLoader {
//        id: fontLoader
//        source: "TitilliumWeb-Regular.ttf"
//    }
//    readonly property alias fontFamily: fontLoader.name
    readonly property int topBarHeight: 50
    readonly property Gradient bottomUpGradient: Gradient {
        GradientStop { position: 0.0; color: "lightsteelblue" }
        GradientStop { position: 1.0; color: "blue" }
    }
}
