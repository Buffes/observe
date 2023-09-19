import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Column {
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: grid.title
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        Button {
           text: "❮"
           width: 40
           onClicked: {
               var month = grid.month - 1
               if (month < 0) {
                   month = 11
                   grid.year -= 1 // NOTE: doesn't handle the BC/AD divide properly.
               }
               grid.month = month
           }
        }

        Button {
           text: "❯"
           width: 40
           onClicked: {
               var month = grid.month + 1
               if (month > 11) {
                   month = 0
                   grid.year += 1
               }
               grid.month = month
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
            month: Calendar.September
            year: 2023
            locale: Qt.locale("en_SE")

            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: Text {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                opacity: model.month === grid.month ? 1 : 0.5
                text: model.day
                font: grid.font


                required property var model
            }

            onClicked: console.log(date)
        }
    }
}
/*Column {
    Row {
        Button {
            id: prev_month_button
            width: parent.width * 0.5
            text: "<"
        }

        Button {
            id: next_month_button
            width: parent.width * 0.5
            text: ">"
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
            month: Calendar.September
            year: 2023
            locale: Qt.locale("sv_SE")

            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: Text {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                opacity: model.month === grid.month ? 1 : 0.5
                text: model.day
                font: grid.font

                required property var model
            }

            onClicked: console.log(date)
        }
    }
}*/
