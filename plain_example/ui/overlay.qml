import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "./component" as Comp

Rectangle {
    color: "transparent"

    ListModel {
        id: shaderListModel
        ListElement {
            text: "Textured Phong (Projection)"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            geometryShaderFile: "resources/shaders/phongcomputenormalsflat.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "textureProjectedPhong"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Plain Phong"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            geometryShaderFile: "resources/shaders/phongcomputenormalsflat.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "plainPhong"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Simple Phong Shader"
            vertexShaderFile: "resources/shaders/phong.vert"
            vertexShaderProc: "plain"
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "plainPhong"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Only Wireframe"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            geometryShaderFile: "resources/shaders/phongcomputenormalsflat.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "wireFrame"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Fur"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            geometryShaderFile: "resources/shaders/fur.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "furPhong"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Tesselation using Textures"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            tesselationControlShaderFile: "resources/shaders/tess.tcs"
            tesselationControlShaderProc: ""
            tesselationEvaluationShaderFile: "resources/shaders/tess.tes"
            tesselationEvaluationShaderProc: ""
            geometryShaderFile: "resources/shaders/phongcomputenormalsflat.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "bumpyPhong"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "TessNormals"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            tesselationControlShaderFile: "resources/shaders/tess.tcs"
            tesselationControlShaderProc: ""
            tesselationEvaluationShaderFile: "resources/shaders/tess.tes"
            tesselationEvaluationShaderProc: ""
            geometryShaderFile: "resources/shaders/phongcomputenormalsflat.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "showNormals"
            uniforms: "phongUniforms"
        }
        ListElement {
            text: "Show Tangent Space"
            vertexShaderFile: "resources/shaders/passthrough.vert"
            vertexShaderProc: ""
            geometryShaderFile: "resources/shaders/showTangentSpace.geom"
            geometryShaderProc: ""
            fragmentShaderFile: "resources/shaders/phong.frag"
            fragmentShaderProc: "plainPhong"
            uniforms: "phongUniforms"
        }
    }

    ListModel {
        id: phongUniformsListModel
        ListElement {
            name: "Material Ka"
            uniformName: "material.Ka"
            isVector: true
            defaultValue: 0.1
            minValue: 0
            maxValue: 3
            step: 0.001
        }
        ListElement {
            name: "Material Kd"
            uniformName: "material.Kd"
            isVector: true
            defaultValue: 0.8
            minValue: 0
            maxValue: 3
            step: 0.001
        }
        ListElement {
            name: "Use Specular Map"
            uniformName: "useSpecularMap"
            isVector: false
            isBool: true
            defaultChecked: true
        }
        ListElement {
            name: "Material Ks"
            uniformName: "material.Ks"
            isVector: true
            defaultValue: 2.0
            minValue: 0
            maxValue: 10
            stee: 0.001
        }
        ListElement {
            name: "Light Position"
            uniformName: "light.position"
            isVector: true
            defaultValue: 30.0
            minValue: -5
            maxValue: 5
            step: 0.001
        }
        ListElement {
            name: "Material Shininess (log)"
            uniformName: "material.shininess"
            isVector: false
            defaultValue: 5.0
            minValue: 0
            maxValue: 10
            step: 0.01
        }
        ListElement {
            name: "Smooth Shading"
            uniformName: "doSmooth"
            isVector: false
            isBool: true
            defaultChecked: true
        }
        ListElement {
            name: "Opacity (Alpha)"
            uniformName: "alpha"
            isVector: false
            defaultValue: 1.0
            minValue: 0
            maxValue: 1
            step: 0.001
        }
        ListElement {
            name: "Explode"
            uniformName: "explosionFactor"
            isVector: false
            defaultValue: 0.0
            minValue: -0.02
            maxValue: 0.02
            step: 0.001
        }
        ListElement {
            name: "Scale Faces on Explode"
            uniformName: "explosionDoFaceScale"
            isVector: false
            isBool: true
            defaultChecked: true
        }
        ListElement {
            name: "Wireframe"
            uniformName: "doWireframe"
            isVector: false
            isBool: true
            defaultChecked: false
        }
        ListElement {
            name: "Wireframe Color"
            uniformName: "line.color"
            isVector: true
            defaultValue: 1.0
            minValue: 0
            maxValue: 1
            stee: 0.01
        }
        ListElement {
            name: "Wireframe Opacity"
            uniformName: "wireframeAlpha"
            isVector: false
            defaultValue: 0.5
            minValue: 0
            maxValue: 1
            step: 0.001
        }
        ListElement {
            name: "Wireframe Thickness"
            uniformName: "line.width"
            isVector: false
            defaultValue: 1.0
            minValue: 0
            maxValue: 5
            step: 0.001
        }
        ListElement {
            name: "Fur Levels"
            uniformName: "renderPasses"
            isVector: false
            isInt: true
            defaultValue: 5
            minValue: 0
            maxValue: 18
            step: 1
        }
        ListElement {
            name: "Tess Level Inner"
            uniformName: "tessLevelInner"
            isVector: false
            isInt: false
            defaultValue: 5
            minValue: 1
            maxValue: 50
            step: 1
        }
        ListElement {
            name: "Tess Level Outer"
            uniformName: "tessLevelOuter"
            isVector: false
            isInt: false
            defaultValue: 5
            minValue: 1
            maxValue: 50
            step: 1
        }
        ListElement {
            name: "Tess Scale"
            uniformName: "tessScale"
            isVector: false
            isInt: false
            defaultValue: 0.01
            minValue: 0
            maxValue: 3
            step: 0.0001
        }
        ListElement {
            name: "Bump normal influence"
            uniformName: "bumpFactor"
            isVector: false
            isInt: false
            defaultValue: 0.01
            minValue: 0
            maxValue: 2
            step: 0.0001
        }
    }

    Rectangle {
        id: rootRectangle
        width: 220
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color.a: 0.5
        //color: "transparent"

        Flickable {
            anchors.fill: parent
            contentHeight: controlsColumn.height
            contentWidth: width

            ColumnLayout  {
                //anchors.fill: parent
                id: controlsColumn
                Button {
                    id: compileBtn
                    text: "Compile Shader"
                    onClicked: {
                        console.log("recompiling shader");
                        application.reloadShader();
                    }
                }
                //Button {
                //    id: toggleUiBtn
                //    text: "Toggle ui"
                //    onClicked: {
                //        console.log("toggle ui");
                //        application.toggleDialog();
                //    }
                //}
                GroupBox {
                    id: cameraModeGroupBox
                    ///anchors.left: parent.left
                    ///anchors.right: parent.right
                    title: "Camera Mode"
                    ExclusiveGroup { id: cameraMode }
                    ColumnLayout {
                        RadioButton {
                            id: walkthroughRadio
                            text: "Walkthrough"
                            exclusiveGroup: cameraMode
                            checked: false
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
                            checked: true
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
                    id: objectGroupBox
                    title: "Object"
                    ExclusiveGroup { id: currentObject }
                    ColumnLayout {
                        RadioButton {
                            id: bunnyRadio
                            text: "Bunny"
                            exclusiveGroup: currentObject
                            checked: true
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("object: "+text);
                                    application.setObjectBunny();
                                }
                            }
                        }

                        RadioButton {
                            id: planeRadio
                            text: "Plane"
                            exclusiveGroup: currentObject
                            onCheckedChanged: {
                                if(checked) {
                                    console.log("object: "+text);
                                    application.setObjectPlane();
                                }
                            }
                        }
                    }
                }
                CheckBox {
                    id: rotationCb
                    ///anchors.left: parent.left
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
                    ///anchors.left: parent.left
                    ///anchors.right: parent.right
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
                            text: "Disable"
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
                Label {
                    text: "GL_TEXTURE_MIN_FILTER"
                }
                ComboBox {
                    id: minTexfilter
                    model: appGlTexMinFilter
                    onCurrentIndexChanged: {
                        application.selectedMinFilter = appGlTexMinFilter[currentIndex];
                    }

                }
                Label {
                    text: "GL_TEXTURE_MAG_FILTER"
                }
                ComboBox {
                    id: magTexfilter
                    model: appGlTexMagFilter
                    onCurrentIndexChanged: {
                        application.selectedMagFilter = appGlTexMagFilter[currentIndex];
                    }
                }
                Label {
                    id: labeledUniformLabel
                    text: "Anisotropy: " + anisotrophySlider.value.toFixed(0)
                }
                Slider {
                    id: anisotrophySlider
                    implicitWidth: rootRectangle.width-10
                    value: 16
                    maximumValue: 32
                    minimumValue: 1
                    stepSize: 1
                    onValueChanged: {
                        application.anisotropy = value;
                    }
                }
                Label {
                    text: "Shader"
                }
                ComboBox {
                    id: shaderSelectionComboBox
                    model: shaderListModel
                    onCurrentIndexChanged: {
                        console.log(shaderListModel.get(currentIndex).text);
                        application.selectedShader = shaderListModel.get(currentIndex);
                    }
                }

                Component {
                    id: shaderUniformDelegate
                    ColumnLayout {
                        spacing: 0
                        RowLayout {
                            spacing: 5
                            Label {
                                id: labeledUniformLabel
                                text: name + ": " + labeledUniformSlider.value.toFixed(2) + (isVector?", " + labeledUniformSlider2.value.toFixed(2) + ", " + labeledUniformSlider3.value.toFixed(2) :"")
                                font.bold: isBool?boolCb.checked!==defaultChecked:labeledUniformSlider.value!==defaultValue||(isVector&&(labeledUniformSlider2.value!==defaultValue||labeledUniformSlider3.value!==defaultValue))
                            }
                            CheckBox {
                                id: linkedCb
                                text: "fix"
                                visible: isVector
                                checked: true
                            }
                        }
                        Slider {
                            ///anchors.horizontalCenter: parent.horizontalCenter
                            id: labeledUniformSlider
                            visible: !isBool
                            implicitWidth: rootRectangle.width-10
                            value: defaultValue
                            maximumValue: maxValue
                            minimumValue: minValue
                            stepSize: step
                            onValueChanged: {
                                if(isVector) {
                                    application.setShaderUniformValue3f(uniformName, value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                } else {
                                    if(isInt)
                                        application.setShaderUniformValue1i(uniformName, value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                }
                                if(linkedCb.checked) {
                                    labeledUniformSlider2.value = value;
                                    labeledUniformSlider3.value = value;
                                }
                            }
                        }
                        Slider {
                            ///anchors.horizontalCenter: parent.horizontalCenter
                            id: labeledUniformSlider2
                            visible: isVector && !isBool
                            implicitWidth: rootRectangle.width-10
                            value: defaultValue
                            maximumValue: maxValue
                            minimumValue: minValue
                            stepSize: step
                            onValueChanged: {
                                if(isVector)
                                    application.setShaderUniformValue3f(uniformName, labeledUniformSlider.value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                else {
                                    if(isInt)
                                        application.setShaderUniformValue1i(uniformName, value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                }
                                if(linkedCb.checked) {
                                    labeledUniformSlider.value = value;
                                    labeledUniformSlider3.value = value;
                                }

                            }
                        }
                        Slider {
                            ///anchors.horizontalCenter: parent.horizontalCenter
                            id: labeledUniformSlider3
                            visible: isVector && !isBool
                            implicitWidth: rootRectangle.width-10
                            value: defaultValue
                            maximumValue: maxValue
                            minimumValue: minValue
                            stepSize: step
                            onValueChanged: {
                                if(isVector)
                                    application.setShaderUniformValue3f(uniformName, labeledUniformSlider.value, labeledUniformSlider2.value, labeledUniformSlider3.value);
                                else {
                                    if(isInt)
                                        application.setShaderUniformValue1i(uniformName, value);
                                    else
                                        application.setShaderUniformValue1f(uniformName, value);
                                }
                                if(linkedCb.checked) {
                                    labeledUniformSlider.value = value;
                                    labeledUniformSlider2.value = value;
                                }
                            }
                        }
                        CheckBox {
                            id: boolCb
                            visible: isBool
                            checked: defaultChecked
                            onCheckedChanged: {
                                if(isBool)
                                {
                                    console.log("Set: "+uniformName+" to "+checked+" (1b)");
                                    application.setShaderUniformValue1b(uniformName, checked);
                                }
                            }
                        }
                    }
                }
                Repeater {
                    model: phongUniformsListModel
                    delegate: shaderUniformDelegate
                }
            }
        }
    }
}
