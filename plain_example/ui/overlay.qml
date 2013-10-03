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
            defaultValue: 0.1
            minValue: 0
            maxValue: 3
            step: 0.1
        }
        ListElement {
            name: "Material Kd"
            uniformName: "material.Kd"
            isVector: true
            defaultValue: 0.7
            minValue: 0
            maxValue: 3
            step: 0.1
        }
        ListElement {
            name: "Material Ks"
            uniformName: "material.Ks"
            isVector: true
            defaultValue: 1.0
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
            defaultValue: 100.0
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
//		anchors.top: parent.top
//		anchors.bottom: parent.bottom
        //color.a: 0.5
        color: "transparent"
        GroupBox {
            //anchors.horizontalCenter: parent.horizontalCenter
            //anchors.bottom: parent.bottom
            //anchors.top: parent.top
            anchors.fill: parent
            title: "Controls"
            ColumnLayout {
                anchors.fill: parent
                id: columnlayout1
                //spacing: 10
				Button {
                    id: compileBtn
                    //anchors.left: parent.left
                    //anchors.top: parent.top
					text: "Compile Shader"
					onClicked: {
						console.log("recompiling shader");
						application.reloadShader();
					}
				}
				Button {
                    id: toggleUiBtn
                    //anchors.left: parent.left
                    //anchors.top: compileBtn.bottom
					text: "Toggle ui"
					onClicked: {
						console.log("toggle ui");
						application.toggleDialog();
					}
				}
                GroupBox {
                    id: cameraModeGroupBox
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //anchors.top: toggleUiBtn.bottom
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
                CheckBox {
                    id: rotationCb
                    anchors.left: parent.left
                    //anchors.top: cameraModeGroupBox.bottom
                    text: "Rotate"
                    onCheckedChanged: {
                        console.log("rotation: "+checked);
                        if(checked)
                            application.enableRotation();
                        else
                            application.disableRotation();
                    }
                }
                GroupBox {
                    id: cullModeGroupBox
                    anchors.left: parent.left
                    anchors.right: parent.right
                    // anchors.top: rotationCb.bottom
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
                    //anchors.left: parent.left
                    //anchors.top: cullModeGroupBox.bottom
                    model: shaderListModel
                    onCurrentIndexChanged: {
                        console.log(shaderListModel.get(currentIndex).text);
                        application.selectedShader = shaderListModel.get(currentIndex);
                    }
                }

                Component {
                    id: shaderUniformDelegate
                    RowLayout {
                        ColumnLayout {
                            RowLayout {
                                spacing: 5
                                Label {
                                    id: labeledUniformLabel
                                    text: name + ": " + labeledUniformSlider.value.toFixed(2) + (isVector?", " + labeledUniformSlider2.value.toFixed(2) + ", " + labeledUniformSlider3.value.toFixed(2) :"")
                                }
                                CheckBox {
                                    id: linkedCb
                                    text: "fix"
                                    visible: isVector
                                }
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
                                        application.setShaderUniformValue3f(uniformName, value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                    if(linkedCb.checked) {
                                        labeledUniformSlider2.value = value;
                                        labeledUniformSlider3.value = value;
                                    }
                                }
                            }
                            Slider {
                                anchors.horizontalCenter: parent.horizontalCenter
                                id: labeledUniformSlider2
                                visible: isVector
                                implicitWidth: rootRectangle.width-10
                                value: defaultValue
                                maximumValue: maxValue
                                minimumValue: minValue
                                stepSize: step
                                onValueChanged: {
                                    if(isVector)
                                        application.setShaderUniformValue3f(uniformName, labeledUniformSlider.value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                    if(linkedCb.checked) {
                                        labeledUniformSlider.value = value;
                                        labeledUniformSlider3.value = value;
                                    }

                                }
                            }
                            Slider {
                                anchors.horizontalCenter: parent.horizontalCenter
                                id: labeledUniformSlider3
                                visible: isVector
                                implicitWidth: rootRectangle.width-10
                                value: defaultValue
                                maximumValue: maxValue
                                minimumValue: minValue
                                stepSize: step
                                onValueChanged: {
                                    if(isVector)
                                        application.setShaderUniformValue3f(uniformName, labeledUniformSlider.value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                    if(linkedCb.checked) {
                                        labeledUniformSlider.value = value;
                                        labeledUniformSlider2.value = value;
                                    }
                                }
                            }
                        }
                    }
                }
                //Row {
                    //anchors.top: shaderSelectionComboBox.bottom
                    //anchors.bottom: parent.bottom
                    Repeater {
                        model: phongUniformsListModel
                        delegate: shaderUniformDelegate
                        //anchors.fill: parent
                        //spacing: 10
                    }
                //}
            }
		}
	}
}
