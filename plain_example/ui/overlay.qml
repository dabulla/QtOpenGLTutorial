import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0


Rectangle {
	//color: "transparent"
	color.a: 0.2
	property alias material_Ka: materialKaSlider.value;
	property alias material_Kd: materialKdSlider.value;
	property alias material_Ks: materialKsSlider.value;
	property alias material_shininess: materialShininessSlider.value;
	property alias lightTheta: lightSlider.value;
	Rectangle {
		width: 200
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		color.a: 0.5
		GroupBox {
			anchors.horizontalCenter: parent.horizontalCenter
			title: "controls"
			ColumnLayout {
				Button {
					anchors.horizontalCenter: parent.horizontalCenter
					text: "Compile Shader"
					onClicked: {
						console.log("recompiling shader");
						application.reloadShader();
					}
				}
				Button {
					anchors.horizontalCenter: parent.horizontalCenter
					text: "Toggle ui"
					onClicked: {
						console.log("toggle ui");
						application.toggleDialog();
					}
				}
				Label {
					text: "Ka: " + materialKaSlider.value
				}
				Slider {
					id: materialKaSlider
					implicitWidth: 150
					value: 0.1
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.1
				}
				Label {
					text: "Kd: " + materialKdSlider.value
				}
				Slider {
					id: materialKdSlider
					implicitWidth: 150
					value: 0.5
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.1
				}
				Label {
					text: "Ks: " + materialKsSlider.value
				}
				Slider {
					id: materialKsSlider
					implicitWidth: 150
					value: 0.3
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.1
				}
				Label {
					text: "Shininess: " + materialShininessSlider.value
				}
				Slider {
					id: materialShininessSlider
					implicitWidth: 150
					value: 10.0
					maximumValue: 100
					minimumValue: 0
					stepSize: 0.1
				}
				Label {
					text: "Light: " + lightSlider.value + "°"
				}
				Slider {
					id: lightSlider
					implicitWidth: 150
					value: 30.0
					maximumValue: 180
					minimumValue: 0
					stepSize: 1
				}
			}
		}
	}
}