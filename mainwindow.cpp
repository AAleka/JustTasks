#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <string_view>
#include <random>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++)
            ss << dis(gen);

        ss << "-";
        for (i = 0; i < 4; i++)
            ss << dis(gen);

        ss << "-4";
        for (i = 0; i < 3; i++)
            ss << dis(gen);

        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++)
            ss << dis(gen);

        ss << "-";
        for (i = 0; i < 12; i++)
            ss << dis(gen);

        return ss.str();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::filesystem::create_directories("./db");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./db/justtasks");

    bool successful_db_connection = db.open();

    if (successful_db_connection){
        if (!db.tables().contains( QLatin1String("justtasks"))) {
            QSqlQuery create_query;

            QString create_table = "CREATE TABLE tasks("
                            "task_id varchar(37), "
                            "task_description varchar(255), "
                            "task_date datetime);";

            if (create_query.exec(create_table)){
                QString insert_into_table = "insert into tasks values(?, ?, ?);";
                QSqlQuery insert_query;

                insert_query.prepare(insert_into_table);

                insert_query.addBindValue("00000000-0000-0000-0000-000000000000");
                insert_query.addBindValue("Fill this JustTasks app.");
                insert_query.addBindValue(QDateTime::currentDateTime().toString("dd-MM-yyyy"));

                if (insert_query.exec()){
                    ui->outputText->setText("Database connected and table created.");
                }
            }
        }
        else{
            ui->outputText->setText("Database connected and table opened.");
        }
    }
    else{
        ui->outputText->setText("Database not connected.");
    }

    QObject::connect(ui->addButton, &QPushButton::clicked, this, [this]() {
        QString task = ui->inputText->toPlainText();
        QString encrypted_task = QString::fromStdString(task.toStdString());
        QString task_id = QString::fromStdString(uuid::generate_uuid_v4());

        QString insert_into_table = "insert into tasks values(?, ?, ?);";
        QSqlQuery insert_query;

        insert_query.prepare(insert_into_table);

        insert_query.addBindValue(task_id);
        insert_query.addBindValue(encrypted_task);
        insert_query.addBindValue(QDateTime::currentDateTime().toString("dd-MM-yyyy"));

        if (insert_query.exec()){
            ui->inputText->setPlainText("");
            ui->outputText->setText("Successfully added: " + task);
        }
    });

    QObject::connect(ui->searchButton, &QPushButton::clicked, this, [this]() {
        ui->outputText->setText("Found tasks:");
        QString keyword = ui->inputText->toPlainText();

        QString select_from_table = "select task_description, task_date from tasks "
                                    "where task_description like \"\%" + keyword + "\%\";";
        QSqlQuery select_query;

        if (select_query.exec(select_from_table)){
            int n{0};
            while (select_query.next()){
                QString select = "(" + select_query.value(1).toString() + ")\t" + select_query.value(0).toString();
                ui->outputText->QTextBrowser::append(QString::number(++n) + ") " + select);
            }
        }
    });

    QObject::connect(ui->deleteButton, &QPushButton::clicked, this, [this](){
        QString keyword = ui->inputText->toPlainText();

        if (keyword != ""){
            ui->outputText->setText("Are you sure you want to delete these tasks?:");
            QString keyword = ui->inputText->toPlainText();

            QString select_from_table = "select task_description, task_date from tasks "
                                        "where task_description like \"\%" + keyword + "\%\";";
            QSqlQuery select_query;

            if (select_query.exec(select_from_table)){
                int n{0};
                while (select_query.next()){
                    QString select = "(" + select_query.value(1).toString() + ")\t" + select_query.value(0).toString();
                    ui->outputText->QTextBrowser::append(QString::number(++n) + ") " + select);
                }
            }

            ui->outputText->QTextBrowser::append("\nPress delete button to confirm");
            QObject::connect(ui->deleteButton, &QPushButton::clicked, this, [this, keyword](){
                QString delete_tasks = "delete from tasks where task_description like \"\%" + keyword + "\%\";";
                QSqlQuery delete_query;

                if(delete_query.exec(delete_tasks)){
                    ui->outputText->setText("Tasks have been deleted.");
                    ui->inputText->setPlainText("");
                }
            });
        }
        else{
            ui->outputText->setText("No keyword was provided.");
        }
    });

    QObject::connect(ui->actionExport_to_txt, &QAction::triggered, this, [this, successful_db_connection](){
        std::ofstream file;
        file.open("tasks.txt");

        if (successful_db_connection){

            QString select_tasks = "Select task_description, task_date from tasks;";
            QSqlQuery select_query;

            if (select_query.exec(select_tasks)){
                while (select_query.next()){
                    file << "(" + select_query.value(1).toString().toStdString() + ")\t" + select_query.value(0).toString().toStdString() + '\n';
                }

            }
        }

        file.close();
        ui->outputText->setText("Successfully exported to tasks.txt");
    });
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}
