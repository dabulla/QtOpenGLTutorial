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

    ListModel {
        id: phongUniforms
        ListElement {
            name: "Material Ka"
            uniformName: "material.Ka"
            defaultValue: "0.5"
            min: 0;
            max: 100;
            step: 0.1;
        }
        ListElement {
            name: "Material Kd"
            uniformName: "material.Kd"
            defaultValue: "0.2"
            min: 0;
            max: 100;
            step: 0.1;
        }
        ListElement {
            name: "Material Ks"
            uniformName: "material.Ks"
            defaultValue: "0.2"
            min: 0;
            max: 100;
            step: 0.1;
        }
    }
	ListModel {
		id: shaderListModel
		ListElement {
            text: "Phong Shader"
            vertexShaderFile: "resources/shaders/phong.vert"
			vertexShaderProc: "main"
            fragmentShaderFile: "resources/shaders/phong.frag"
			fragmentShaderProc: "main"
            uniforms: phongUniforms
		}
		ListElement {
            text: "Textured Phong Shader"
            vertexShaderFile: "resources/shaders/phong.vert"
			vertexShaderProc: "main"
            fragmentShaderFile: "resources/shaders/phong.frag"
			fragmentShaderProc: "textured"
            uniforms: phongUniforms
		}
	}


    Rectangle {
        width: 200
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
        //color.a: 0.5
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
                ComboBox {
                    id: shaderSelectionComboBox
                    model: shaderListModel
                    onCurrentIndexChanged: {
                        console.log("changed");
                        console.log(shaderListModel.get(currentIndex));
                        console.log(shaderListModel.get(currentIndex).text);
                        application.selectedShader = shaderListModel.get(currentIndex);
                    }
                }
/*				Label {
					text: "Ka: " + materialKaSlider.value
				}
				Slider {
					id: materialKaSlider
					implicitWidth: 150
					value: 0.1
					maximumValue: 1
					minimumValue: 0
					stepSize: 0.1
                    onValueChanged: application.setShaderUniformValue("material.Ka", value)
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
*/
                Component {
                    id: shaderUniformDelegate
                    Item {
                        width: 180; height: 40
                        Column {
                            Label { text: name }
                            Slider {
                                implicitWidth: 150
                                value: defaultValue
                                maximumValue: maximumValue
                                minimumValue: minimumValue
                                stepSize: stepSize
                            }
                        }
                    }
                }

                ListView {
                    anchors.fill: parent
                    model: shaderListModel
                    delegate: shaderUniformDelegate
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    focus: true
                }
			}
		}
	}
}
