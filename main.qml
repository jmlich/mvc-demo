import QtQuick 2.14
import QtCharts 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtLocation 5.12
import QtPositioning 5.12
import cz.mlich 1.0

ApplicationWindow {
    id: application
    width: 1024
    height: 480
    visible: true
    title: qsTr("Model View Control demo")
    property string localDataFile: "./salary.json"

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
        text: "22.89";
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
            height: 30;

            Button {
                anchors.left: parent.left;
                width: parent.width/2
                text: country
                onClicked: {
                    map.center = QtPositioning.coordinate(latitude, longitude)
                }
            }
            TextField {
                width: parent.width/2
                anchors.right: parent.right;
                text: nominal
                selectByMouse: true;

                onAccepted: {
                    salaryModel.setProperty(index, "nominal", text)
                    console.log("changing nominal to: " + text)
                }
            }

        }
    }


    Map {
        id: map;
        height: parent.height;
        anchors.left: listview.right
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: parent.width - listview.width;
        zoomLevel: 3;
        plugin:     Plugin {
            name: "osm"
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
                        text: toCzk(nominal, exchangeRate.text);
                        color: "#cc0000"
                        font.bold: true
                    }
                }
            }
        }
    }

    function toCzk(num, exange) {
        return Number(exange * num / 12).toLocaleCurrencyString(Qt.locale("cs_CZ"), ' CZK');
    }

    function loadRemote() {
        var http = new XMLHttpRequest();
        http.onreadystatechange = function() {
            if (http.readyState === XMLHttpRequest.DONE) {
                console.log("download done")
                jsonToSalaryModel(http.responseText);
            }
        }

        http.open("GET", "https://pcmlich.fit.vutbr.cz/salary.json");
        http.send();
    }

    function jsonToSalaryModel(json_string) {
        try {
            var data = JSON.parse(json_string)
            salaryModel.clear();
            for (var i = 0; i < data.length; i++) {
                salaryModel.append(data[i])
            }
            console.log(salaryModel.count + " items was loaded")
        } catch (e) {
            console.error(e + json_string)
        }
    }

    function salaryModelToJSON() {
        var data = []
        for (var i = 0; i < salaryModel.count; i++) {
            var item = salaryModel.get(i)
            data.push(item);
        }
        if (salaryModel.count < 3) {
            throw "salaryModel.count < 3"
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
        file.write_local(localDataFile, salaryModelToJSON())
    }

}
