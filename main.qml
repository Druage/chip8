import QtQuick 2.7
import QtQuick.Controls 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import Chip8 1.0

Window {
    visible: true
    width: 640
    height: 320
    title: qsTr("Chip8")

    color: "#454444";

    DropShadow {
        id: dropShadow;
        source: video;
        anchors.fill: source;
        horizontalOffset: 0
        verticalOffset: 2
        radius: 8.0
        samples: radius * 2;
        color: "#80000000"
    }

    VideoOutput {
        id: video;
        focus: true;
        anchors.fill: parent;
    }

}
