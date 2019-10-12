#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cstdint>
#include <vector>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this] (QString const& new_text)
    {
        bg_thread.set_number_to_factor(new_text.toULongLong());
    });

    connect(&bg_thread, &background_thread::result_changed, this, [this]
    {
        auto res = bg_thread.get_result();
        
        QString text;
        bool first = true;
        for (uint64_t a : res.factors)
        {
            if (!first)
                text += " * ";

            text += QString("%1").arg(a);
            first = false;
        }

        if (res.incomplete)
        {
            if (!first)
                text += " * ";

            text += "...";
        }
        ui->lineEdit_3->setText(text);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
            bool first = true;
            for (uint64_t a : factors)
            {
                if (!first)
                    std::cout <<  " * ";

                std::cout << a;
                first = false;
            }

            if (cancel.load())
            {
                if (!first)
                    std::cout <<  " * ";

                std::cout << "...";
            }
*/

/*auto factors = factor();

QString result;

bool first = true;
for (uint64_t a : factors)
{
    if (!first)
        result += " * ";

    result += QString("%1").arg(a);
    first = false;
}

ui->lineEdit_3->setText(result);*/

// 23234747441283
