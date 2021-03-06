import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "./component" as Comp

Item {
    id: shaderUniformDelegate
    property alias defValue: defaultValue
    ColumnLayout {
        spacing: 0
        RowLayout {
            spacing: 5
            Label {
                id: labeledUniformLabel
                text: name + (isBool?"":(": " + labeledUniformSlider.value.toFixed(2) + (isVector?", " + labeledUniformSlider2.value.toFixed(2) + ", " + labeledUniformSlider3.value.toFixed(2) :"")))
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
            minimumValue: minValue
            maximumValue: maxValue
            value: parent.defValue
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
