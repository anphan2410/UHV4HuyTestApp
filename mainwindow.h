#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <serialinterface.h>
#include <windowcommand.h>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    WindowCommand *NowUHV4;

    void listSerialPortsToComboBox();
    void serialInterfaceIsRequested();
    void HiddenOutUi(bool IsDisabled);
    void AutoSendnRead();
    void PauseAutoSendnRead();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:    
    void serialInterfaceIsConnected();
    void serialInterfaceIsDisconnected();
    void serialInterfaceInvalid();

    void displayMessageSent(const QByteArray &MessageSent);
    void displayMessageRead(const QByteArray &MessageRead);
    void displayBufferCount(int Count);

    void on_pushButton_SerialConnect_clicked();
    void on_pushButton_clicked();
    void on_pushButton_7_clicked();
    void on_comboBoxSerial_currentIndexChanged(const QString &arg1);
    void on_pushButton_2_clicked();
    void on_spinBox_9_valueChanged(int arg1);
    void on_spinBox_10_valueChanged(int arg1);
    void on_pushButton_6_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

    void ToggleDisplayHighlightSendRead(quint8 option = 0);
signals:
    void sigStartSerialInterfaceThreadJob();
    void sigReConfiguretheSerialInterface(QString);
    void sigSerialInterfaceClosureRequest();
    void sigSerialInterfaceBufferClear();
    void sigSerialInterfaceStartSendReadLoop();
    void sigSerialInterfacePauseSendReadLoop();
    void sigSerialInterfaceAddAPairToCommandList(APairOfPrioAndCommand);
    void sigSerialInterfaceAddAListToCommandList(AListOfPrioAndCommand);
};

#endif // MAINWINDOW_H
