import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import cz.mlich 1.0

ApplicationWindow {
    id: root
    width: 1024
    height: 480
    visible: true
    title: qsTr("Model View Control demo")
    readonly property string localDataFile: "./salary.json"
    readonly property real monthlyRate: exchangeRate.acceptableInput
                                        ? Number.fromLocaleString(Qt.locale(), exchangeRate.text) / 12 : 0
    readonly property var czkLocale: Qt.locale("cs_CZ")

    FileReader {
        id: file
    }

    ListModel {
        id: salaryModel;
    }

    TextField {
        id: exchangeRate
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        text: Number(22.89).toLocaleString(Qt.locale(), "f", 2)
        placeholderText: qsTr("Exchange rate")
        validator: DoubleValidator { }
        selectByMouse: true;
        z: 1
    }

    ListView {
        id: listview
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: 300;

        model: salaryModel
        delegate: Rectangle {
            width: listview.width;
            height: nominalField.implicitHeight;

            Button {
                anchors.left: parent.left;
                width: parent.width/2
                text: country
                onClicked: {
                    map.center = QtPositioning.coordinate(latitude, longitude)
                }
            }
            TextField {
                id: nominalField
                width: parent.width/2
                anchors.right: parent.right;
                text: nominal
                validator: IntValidator { bottom: 0 }
                selectByMouse: true;

                onEditingFinished: {
                    salaryModel.setProperty(index, "nominal", text)
                    console.log("changing nominal to: " + text)
                }
            }

        }
    }


    Map {
        id: map;
        anchors.left: listview.right
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right
        zoomLevel: 3;
        plugin:     Plugin {
            name: "osm"
        }

        DragHandler {
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }
        WheelHandler {
            acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                             ? PointerDevice.Mouse | PointerDevice.TouchPad
                             : PointerDevice.Mouse
            rotationScale: 1/120
            property: "zoomLevel"
        }
        MapItemView {
            model: salaryModel
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(latitude, longitude)

                anchorPoint.x: image.width * 0.5
                anchorPoint.y: image.height

                sourceItem: Column {
                    Image {
                        id: image;
                        source: "marker-icon.png"
                    }
                    Text {
                        text: toCzk(nominal)
                        textFormat: Text.PlainText
                        color: "#cc0000"
                        font.bold: true
                    }
                }
            }
        }
    }

    function toCzk(num) {
        return Number(num * monthlyRate).toLocaleCurrencyString(czkLocale, "CZK");
    }

    function loadRemote() {
        const http = new XMLHttpRequest();
        http.onreadystatechange = function() {
            if (http.readyState !== XMLHttpRequest.DONE) {
                return
            }
            if (http.status !== 200) {
                console.error("download failed with HTTP status " + http.status)
                return
            }
            console.log("download done")
            jsonToSalaryModel(http.responseText);
        }

        http.open("GET", "https://raw.githubusercontent.com/jmlich/mvc-demo/main/salary.json");
        http.send();
    }

    function jsonToSalaryModel(jsonString) {
        let data
        try {
            data = JSON.parse(jsonString)
        } catch (e) {
            console.error("cannot parse salary data: " + e)
            return
        }
        if (!Array.isArray(data)) {
            console.error("salary data is not an array")
            return
        }
        salaryModel.clear();
        for (let i = 0; i < data.length; i++) {
            salaryModel.append(data[i])
        }
        console.log(salaryModel.count + " items were loaded")
    }

    function salaryModelToJSON() {
        const data = []
        for (let i = 0; i < salaryModel.count; i++) {
            data.push(salaryModel.get(i));
        }
        return JSON.stringify(data);
    }

    Component.onCompleted: {
        if (file.file_exists_local(localDataFile)) {
            jsonToSalaryModel(file.read_local(localDataFile))
        } else {
            loadRemote()
        }
    }

    onClosing: {
        if (salaryModel.count < 3) {
            console.warn("salary data not saved: only " + salaryModel.count + " items loaded")
            return
        }
        if (!file.write_local(localDataFile, salaryModelToJSON())) {
            console.error("cannot save salary data to " + localDataFile)
        }
    }

}
