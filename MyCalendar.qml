import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Column {
    id: calendar
    signal dateSelected(date date)
    property font titleFont

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: grid.title
        font: titleFont
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        Button {
           text: "❮❮"
           width: 40
           onClicked: {
               grid.year -= 1;
           }
        }

        Button {
           text: "❮"
           width: 40
           onClicked: {
               var month = grid.month - 1;
               if (month < 0) {
                   month = 11;
                   grid.year -= 1; // NOTE: doesn't handle the BC/AD divide properly.
               }
               grid.month = month;
           }
        }

        Button {
           text: "❯"
           width: 40
           onClicked: {
               var month = grid.month + 1;
               if (month > 11) {
                   month = 0;
                   grid.year += 1;
               }
               grid.month = month;
           }
        }

        Button {
           text: "❯❯"
           width: 40
           onClicked: {
               grid.year += 1;
           }
        }
    }
    GridLayout {
        columns: 2
        DayOfWeekRow {
            locale: grid.locale

            Layout.column: 1
            Layout.fillWidth: true
        }

        WeekNumberColumn {
            month: grid.month
            year: grid.year
            locale: grid.locale

            Layout.fillHeight: true
        }

        MonthGrid {
            id: grid
            month: grid.selected_date.getMonth()
            year: grid.selected_date.getFullYear()
            locale: Qt.locale("en_SE")

            Layout.fillWidth: true
            Layout.fillHeight: true

            property var selected_date: new Date();

            delegate: Text {
                id: month_day_text
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                opacity: model.month === grid.month ? 1 : 0.5
                text: model.day
                font.family: grid.font.family
                font.pointSize: grid.font.pointSize
                color: model.day === grid.selected_date.getDate() && model.month === grid.selected_date.getMonth() ? "red" : "black"

                required property var model
            }

            onClicked: date => {
                           grid.selected_date = date;
                           calendar.dateSelected(date);
                       }

            Component.onCompleted: calendar.dateSelected(grid.selected_date);
        }
    }

    SpinBox {
        editable: true
        from: 0
        to: 24
        stepSize: 1
    }

    Button {

        text: "Set to today"
        onClicked: {
            let today = new Date();
            grid.selected_date = today;
            calendar.dateSelected(today);

       }
    }
}
