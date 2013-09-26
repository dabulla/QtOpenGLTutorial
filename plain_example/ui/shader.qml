import QtQuick 2.0
import QtMultimedia 5.0
import QtQuick.Controls 1.0

Rectangle {
    width: 600
    height: 400
	color: "transparent"
 //   /*MediaPlayer {
 //       id: mediaPlayer
    //    autoplay: true
      //  source: "test.ogg"
    //}*/
	
    Button {
        id: mediaPlayer
        anchors.centerIn: parent
        text: "Test"
        //onClicked: text = "pushed"
        onClicked: {
            root.color = "khaki";
            console.log("a click");
        }
    }
	
    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        source: mediaPlayer
    }

    ShaderEffect {
        anchors.fill: parent

        // Properties which will be passed into the shader as uniforms
        property real amplitude: 0.02
        property real frequency: 20
        property real time: 0

        NumberAnimation on time {
            loops: Animation.Infinite
            from: 0
            to: Math.PI * 2
            duration: 600
        }

        property variant source: ShaderEffectSource {
            sourceItem: videoOutput
            hideSource: true
        }

        fragmentShader: "
            uniform highp float amplitude;
            uniform highp float frequency;
            uniform highp float time;
            uniform sampler2D source;
            uniform lowp float qt_Opacity;
            varying highp vec2 qt_TexCoord0;
            void main() {
                highp vec2 p = sin(time + frequency * qt_TexCoord0);
                highp vec2 tc = qt_TexCoord0 + amplitude * vec2(p.y, -p.x);
                gl_FragColor = qt_Opacity * texture2D(source, tc);
            }
        "
    }
}