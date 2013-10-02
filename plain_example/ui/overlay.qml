import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0


Rectangle {
    color: "transparent"
    //color.a: 0.2
//	property alias material_Ka: materialKaSlider.value;
//	property alias material_Kd: materialKdSlider.value;
//	property alias material_Ks: materialKsSlider.value;
//	property alias material_shininess: materialShininessSlider.value;
//  property alias lightTheta: lightSlider.value;

    ListModel {
        id: phongUniformsListModel
        ListElement {
            name: "Material Ka"
            uniformName: "material.Ka"
            isVector: true
            defaultValue: 0.5
            minValue: 0
            maxValue: 10
            step: 0.1
        }
        ListElement {
            name: "Material Kd"
            uniformName: "material.Kd"
            isVector: true
            defaultValue: 0.2
            minValue: 0
            maxValue: 10
            step: 0.1
        }
        ListElement {
            name: "Material Ks"
            uniformName: "material.Ks"
            isVector: true
            defaultValue: 0.2
            minValue: 0
            maxValue: 10
            stee: 0.1
        }
        ListElement {
            name: "lightTheta"
            uniformName: "lightTheta"
            isVector: false
            defaultValue: 30.0
            minValue: 0
            maxValue: 180
            step: 0.1
        }
        ListElement {
            name: "material.shininess"
            uniformName: "material.shininess"
            isVector: false
            defaultValue: 10.0
            minValue: 0
            maxValue: 1000
            step: 0.01
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
            uniforms: "phongUniforms"
		}
		ListElement {
            text: "Textured Phong Shader"
            vertexShaderFile: "resources/shaders/phong.vert"
			vertexShaderProc: "main"
            fragmentShaderFile: "resources/shaders/phong.frag"
			fragmentShaderProc: "textured"
            uniforms: "phongUniforms"
		}
	}


    Rectangle {
        id: rootRectangle
        width: 200
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
        //color.a: 0.5
		GroupBox {
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.bottom: parent.bottom
//            anchors.top: parent.top
            anchors.fill: parent
			title: "controls"
			ColumnLayout {
                anchors.fill: parent
                id: columnlayout1
				Button {
                    id: compileBtn
					anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
					text: "Compile Shader"
					onClicked: {
						console.log("recompiling shader");
						application.reloadShader();
					}
				}
				Button {
                    id: toggleUiBtn
					anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: compileBtn.bottom
					text: "Toggle ui"
					onClicked: {
						console.log("toggle ui");
						application.toggleDialog();
					}
				}
                GroupBox {
                    id: cameraModeGroupBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: toggleUiBtn.bottom
                    title: "Camera Mode"
                    ExclusiveGroup { id: cameraMode }
                    ColumnLayout {
                        RadioButton {
                            id: walkthroughRadio
                            text: "Walkthrough"
                            exclusiveGroup: cameraMode
                            checked: true
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("camera: "+text);
                                    application.setCamerModeWalkthrough();
                                }
                            }
                        }

                        RadioButton {
                            id: objinspectRadio
                            text: "Object Inspection"
                            exclusiveGroup: cameraMode
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("camera: "+text);
                                    application.setCamerModeObjectInspection();
                                }
                            }
                        }
                    }
                }
                GroupBox {
                    id: cullModeGroupBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: cameraModeGroupBox.bottom
                    title: "Camera Mode"
                    ExclusiveGroup { id: cullMode }
                    ColumnLayout {
                        RadioButton {
                            id: frontRadio
                            text: "GL_FRONT"
                            exclusiveGroup: cullMode
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("cull: "+text);
                                    application.setCullmodeFront();
                                }
                            }
                        }
                        RadioButton {
                            id: backRadio
                            text: "GL_BACK"
                            exclusiveGroup: cullMode
                            checked: true
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("cull: "+text);
                                    application.setCullmodeBack();
                                }
                            }
                        }
                        RadioButton {
                            id: bothRadio
                            text: "GL_FRONT_AND_BACK"
                            exclusiveGroup: cullMode
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("cull: "+text);
                                    application.setCullmodeBoth();
                                }
                            }
                        }
                        RadioButton {
                            id: noneRadio
                            text: "GL_NONE"
                            exclusiveGroup: cullMode
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("cull: "+text);
                                    application.setCullmodeNone();
                                }
                            }
                        }
                    }
                }
                ComboBox {
                    id: shaderSelectionComboBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: cullModeGroupBox.bottom
                    model: shaderListModel
                    onCurrentIndexChanged: {
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
                    Row {
                        Column {
                            Label {
                                id: labeledUniformLabel
                                text: name + ": " + labeledUniformSlider.value
                            }
                            Slider {
                                anchors.horizontalCenter: parent.horizontalCenter
                                id: labeledUniformSlider
                                implicitWidth: rootRectangle.width-10
                                value: defaultValue
                                maximumValue: maxValue
                                minimumValue: minValue
                                stepSize: step
                                onValueChanged: {
                                    if(isVector)
                                        application.setShaderUniformValue3f(uniformName, value, value, value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);

                                }
                            }
                        }
                    }
                }
                Row {
                    anchors.top: shaderSelectionComboBox.bottom
                    anchors.bottom: parent.bottom
                    ListView {
                        model: phongUniformsListModel
                        delegate: shaderUniformDelegate
                        anchors.fill: parent
                    }
                }
//                Component {
//                    id: shaderUniformDelegate
//                    Item {
//                        width: 180
//                        //height: labeledUniformSlider.height + labeledUniformLabel.height
//                        height: 50
//                        ColumnLayout {
//                            height: 50
//                            id: labeledUniformLayout
//                            anchors.fill: parent
//                            Label {
//                                height: 50
//                                id: labeledUniformLabel
//                                text: name
//                            }
//                            Slider {
//                                height: 50
//                                id: labeledUniformSlider
//                                implicitWidth: 150
//                                value: defaultValue
//                                maximumValue: maximumValue
//                                minimumValue: minimumValue
//                                stepSize: stepSize
//                                onValueChanged: {
//                                    application.setShaderUniformValue(uniformName, value);
//                                }
//                            }
//                        }
//                    }
//                }
//                ListView {
//                    interactive: true
//                    flickableDirection: Flickable.HorizontalFlick
//                    transformOrigin: Item.Center
//                    spacing: 0
//                    orientation: ListView.Horizontal
//                    boundsBehavior: Flickable.StopAtBounds
//                    height: 500

//                    model: phongUniformsListModel
//                    delegate: shaderUniformDelegate
//                    focus: true
//                }
			}
		}
	}
}
