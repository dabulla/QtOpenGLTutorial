import QtQuick 2.1
import QtQuick.Controls 1.0

/*
ApplicationWindow {
    width: 360
    height: 360
    Button {
        anchors.centerIn: parent
        text: "Test"
        //onClicked: text = "pushed"
        onClicked: {
            root.color = "khaki";
            console.log("a click");
        }
    }
}

*/
Rectangle {
    width: 360
    height: 360
    Button {
        anchors.centerIn: parent
        text: "Test"
        //onClicked: text = "pushed"
        onClicked: {
            text = "pushed"
            root.color = "khaki";
            console.log("a click");
        }
        onVisibleChanged: {
            root.color = "blue";
        }
    }
}
