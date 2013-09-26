import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0


Rectangle {
	color: "transparent"
	//color.a: 0.2
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
					text: "Compile Shader"
					onClicked: {
						console.log("recompiling shader");
						application.reloadShader();
					}
				}
				Label {
					text: "Ka"
				}
				Slider {
					id: materialKaSlider
					implicitWidth: 150
					value: 0.1
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Label {
					text: "Kd"
				}
				Slider {
					id: materialKdSlider
					implicitWidth: 150
					value: 1.0
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Label {
					text: "Ks"
				}
				Slider {
					id: materialKsSlider
					implicitWidth: 150
					value: 0.3
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.01
				}
				Label {
					text: "Shininess"
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
					text: "Light"
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