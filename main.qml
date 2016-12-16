import QtQuick 2.7
import QtQuick.Controls 1.1
import QtQuick.Window 2.2
import emulator 1.0

Window {
    visible: true
    width: 640
    height: 320
    title: qsTr("Hello World")

    Chip8 {
        id: chip8;
        anchors.fill: parent;
    }

}
