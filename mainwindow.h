#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "huffman.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    HT::huffman huffman;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_copy_clicked();

    void on_pushButtonopen_clicked();

    void on_pushButton_openComp_clicked();

    void on_pushButton_descomp_clicked();

private:
    Ui::MainWindow *ui;
    void fill_table();
    void fill_stats(qint64 originalBytes);
    void fill_bits();
    QString encodedBits;
};
#endif // MAINWINDOW_H
