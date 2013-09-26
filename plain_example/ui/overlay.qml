import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0


Rectangle {
	width: 800
	height: 450
	color: "transparent"
	//color.a: 0.2
	anchors.centerIn: parent
	property alias material_Ka: materialKaSlider.value;
	property alias material_Kd: materialKdSlider.value;
	property alias material_Ks: materialKsSlider.value;
	property alias material_shininess: materialShininessSlider.value;
/*	Rectangle {
		width: 200
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.left: parent.right
		anchors.bottom: parent.bottom
		color.a: 0.5*/
		GroupBox {
			title: "controls"
			ColumnLayout {
				Button {
					text: "Test"
					onClicked: {
						text = "pushed"
						console.log("a click");
					}
				}
				Slider {
					id: materialKaSlider
					implicitWidth: 150
					value: 0.1
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Slider {
					id: materialKdSlider
					implicitWidth: 150
					value: 1.0
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Slider {
					id: materialKsSlider
					implicitWidth: 150
					value: 0.3
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Slider {
					id: materialShininessSlider
					implicitWidth: 150
					value: 10.0
					maximumValue: 100
					minimumValue: 0
					stepSize: 0.1
				}
			}
		//}
	}
}