#include "mainwindow.h"
#include"battery.h"
#include"batteryTypes.h"
#include"batteryStation.h"
#include"StationError.h"


#include <QApplication>
#include<QWidget>
#include<QVBoxLayout>
#include<QInputDialog>
#include<QHBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include <QMessageBox>
#include<QTableWidget>
#include<QPushButton>
#include<QLabel>
#include<QGridLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QWidget window;

    // asking for station size (pop up)

    bool ok;
    int capacity = QInputDialog::getInt(&window, "New station", "Number of slots:",10, 1, 40, 1, &ok);
    if(!ok) return 0;
    station mystation(capacity);

    // defining layout----------


    //main layout
    QVBoxLayout* layout = new QVBoxLayout(&window);

    //button row
    QHBoxLayout *buttonRow = new QHBoxLayout();

    QPushButton *addBtn    = new QPushButton("Add battery");
    QPushButton *issueBtn  = new QPushButton("Issue");
    QPushButton *returnBtn = new QPushButton("Return selected");
    QPushButton *removeBtn = new QPushButton("Remove selected");

    buttonRow->addWidget(addBtn);
    buttonRow->addWidget(issueBtn);
    buttonRow->addWidget(returnBtn);
    buttonRow->addWidget(removeBtn);

    //table
    QTableWidget *table = new QTableWidget(0, 4);
    table->setHorizontalHeaderLabels({"ID", "Type", "SoC", "Status"});
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //status grid
    QGridLayout *grid = new QGridLayout();
    vector<QLabel*> boxes;
    for (int i = 0; i < capacity; i++) {
        QLabel *box = new QLabel();
        box->setFixedSize(80, 50);
        box->setAlignment(Qt::AlignCenter);
        grid->addWidget(box, i / 8, i % 8);
        boxes.push_back(box);
    }

    //adding button row and table to a vertical flex box ie the layout
    layout->addLayout(buttonRow);
    layout->addLayout(grid);
    layout->addWidget(table);



    auto refresh = [&]{
        vector<battery*> list = mystation.getBatteryList();
        int count = 0;
        for(battery* ptr : list){
            if (ptr) count++;
        }
        table->setRowCount(count);
        int row = 0;
        for(battery* ptr : list){
            if(!ptr)continue;
            else{
                table->setItem(row, 0, new QTableWidgetItem(QString::number(ptr->getId())));
                table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ptr->getType())));
                table->setItem(row, 2, new QTableWidgetItem(QString::number(ptr->getSoc(), 'f', 1) + " %"));
                table->setItem(row, 3, new QTableWidgetItem(ptr->getStatus() ? "Issued" : "Charging"));
                row++;
            }
        }
        // update the status grid
        for (int i = 0; i < (int)boxes.size(); i++) {
            battery *b = list[i];
            QString colour = "#9a9a9a";
            QString text = "Empty";

            if (b != nullptr) {
                text = "#" + QString::number(b->getId()) + "\n" +
                       QString::number((int)b->getSoc()) + "%";
                if (b->getStatus())        { colour = "#4a6fa5"; text += " OUT"; }
                else if (b->getSoc() >= 80)  colour = "#2e9e5b";
                else if (b->getSoc() >= 20)  colour = "#e0a526";
                else                         colour = "#d64545";
            }

            boxes[i]->setText(text);
            boxes[i]->setStyleSheet("background:" + colour +
                                    "; color:white; border-radius:6px; font-weight:bold;");
        }
    };

    refresh();
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        mystation.changeAll();     // every battery charges/drains its own way
        refresh();                 // redraw the table
    });
    timer.start(1000);             // every 1000 ms = 1 second
    // helper: ID of the selected table row, or -1 if nothing is selected
    auto selectedId = [&]() -> int {
        int r = table->currentRow();
        if (r < 0 || table->item(r, 0) == nullptr) return -1;
        return table->item(r, 0)->text().toInt();
    };

    // Add
    QObject::connect(addBtn, &QPushButton::clicked, [&]() {
        bool ok;
        QString type = QInputDialog::getItem(&window, "Add battery", "Type:",
                                             {"Li-ion", "Lead-acid"}, 0, false, &ok);
        if (!ok) return;
        double soc = QInputDialog::getDouble(&window, "Add battery", "Starting SoC (%):",
                                             50, 0, 100, 1, &ok);
        if (!ok) return;
        try {
            mystation.addBattery(type == "Li-ion" ? 'l' : 'p', soc);
        } catch (stationError &e) {
            QMessageBox::warning(&window, "Can't add battery", e.what());
        }
        refresh();
    });

    // Issue
    QObject::connect(issueBtn, &QPushButton::clicked, [&]() {
        try {
            battery *b = mystation.issueBattery();
            QMessageBox::information(&window, "Battery issued",
                                     "Battery " + QString::number(b->getId()) + " (" +
                                         QString::fromStdString(b->getType()) + ") issued at " +
                                         QString::number(b->getSoc(), 'f', 1) + " %");
        } catch (stationError &e) {
            QMessageBox::warning(&window, "Can't issue", e.what());
        }
        refresh();
    });

    // Return
    QObject::connect(returnBtn, &QPushButton::clicked, [&]() {
        int id = selectedId();
        if (id == -1) {
            QMessageBox::warning(&window, "Return", "Select a battery in the table first.");
            return;
        }
        try {
            mystation.returnBattery(id);
        } catch (stationError &e) {
            QMessageBox::warning(&window, "Can't return", e.what());
        }
        refresh();
    });

    // Remove
    QObject::connect(removeBtn, &QPushButton::clicked, [&]() {
        int id = selectedId();
        if (id == -1) {
            QMessageBox::warning(&window, "Remove", "Select a battery in the table first.");
            return;
        }
        try {
            mystation.removeBattery(id);
        } catch (stationError &e) {
            QMessageBox::warning(&window, "Can't remove", e.what());
        }
        refresh();
    });

    //running
    window.resize(700, 400);
    window.show();
    return app.exec();

}