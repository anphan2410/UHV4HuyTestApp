#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    NowUHV4(new WindowCommand(0))
{
    ui->setupUi(this);
    listSerialPortsToComboBox();
    QThread *UHV4workerThread = new QThread;
    SerialInterface *UHV4workerSI = new SerialInterface("");
    UHV4workerSI->moveToThread(UHV4workerThread);
    qRegisterMetaType<APairOfPrioAndCommand>("APairOfPrioAndCommand");
    qRegisterMetaType<AListOfPrioAndCommand>("AListOfPrioAndCommand");

    connect(this, SIGNAL(sigReConfiguretheSerialInterface(QString)),UHV4workerSI, SLOT(ReConfigSerialPort(QString)));
    connect(this, SIGNAL(sigStartSerialInterfaceThreadJob()),UHV4workerSI, SLOT(StartThreadJob()));
    connect(this, SIGNAL(sigSerialInterfaceClosureRequest()),UHV4workerSI, SLOT(stopThreadJob()));

    connect(this, SIGNAL(sigSerialInterfaceBufferClear()), UHV4workerSI, SLOT(clearCommandList()));

    connect(this, SIGNAL(sigSerialInterfaceStartSendReadLoop()), UHV4workerSI, SLOT(startSendReadLoop()));
    connect(this, SIGNAL(sigSerialInterfacePauseSendReadLoop()), UHV4workerSI, SLOT(pauseSendReadLoop()));
    connect(this, SIGNAL(sigSerialInterfaceAddAPairToCommandList(APairOfPrioAndCommand))
            , UHV4workerSI, SLOT(addToCommandList(APairOfPrioAndCommand)));
    connect(this, SIGNAL(sigSerialInterfaceAddAListToCommandList(AListOfPrioAndCommand))
            , UHV4workerSI, SLOT(addToCommandList(AListOfPrioAndCommand)));

    connect(UHV4workerSI, SIGNAL(IsConnected()), this, SLOT(serialInterfaceIsConnected()));
    //connect(UHV4workerSI, SIGNAL(IsDisconnected()), this, SLOT(serialInterfaceIsDisconnected()));
    connect(UHV4workerSI, SIGNAL(InvalidConnection()), this, SLOT(serialInterfaceIsDisconnected()));

    connect(UHV4workerSI, SIGNAL(BufferCount(int)), this, SLOT(displayBufferCount(int)));
    connect(UHV4workerSI, SIGNAL(MessageSent(QByteArray)), this, SLOT(displayMessageSent(QByteArray)));
    connect(UHV4workerSI, SIGNAL(MessageRead(QByteArray)), this, SLOT(displayMessageRead(QByteArray)));

    /**
              <Add More Here If Needed>
    **/

    UHV4workerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialInterfaceIsRequested()
{
#ifdef qqdebug
    qDebug() << "inside void serialInterfaceIsRequested()" << endl;
#endif
    ui->pushButton_SerialConnect->setText("Connecting");
    ui->connectionStatus->setText("- - -");
    ui->pushButton_SerialConnect->setDisabled(true);    
    emit sigReConfiguretheSerialInterface(ui->comboBoxSerial->currentText());
    emit sigStartSerialInterfaceThreadJob();
#ifdef qqdebug
    qDebug() << "emit sigReConfiguretheSerialInterface & sigStartSerialInterfaceThreadJob" << endl;
#endif
}

void MainWindow::HiddenOutUi(bool IsDisabled)
{
    ui->frameInterrupt->setDisabled(IsDisabled);
    ui->frame_Monitor->setDisabled(IsDisabled);
    ui->label_21->setDisabled(IsDisabled);
    ui->spinBox_9->setDisabled(IsDisabled);
    ui->spinBox_10->setDisabled(IsDisabled);
}

void MainWindow::AutoSendnRead()
{
    ui->pushButton_7->setText("Pause Test");
    emit sigSerialInterfaceStartSendReadLoop();
}

void MainWindow::PauseAutoSendnRead()
{
    ui->pushButton_7->setText("Run Test");
    emit sigSerialInterfacePauseSendReadLoop();
}

void MainWindow::serialInterfaceIsConnected()
{
#ifdef qqdebug
    qDebug() << "inside void serialInterfaceIsConnected()" << endl;
#endif
    ui->pushButton_SerialConnect->setText("Disconnect");
    ui->connectionStatus->setText("Connected");
    ui->pushButton_SerialConnect->setEnabled(true);
    HiddenOutUi(false);
}


void MainWindow::serialInterfaceIsDisconnected()
{
#ifdef qqdebug
    qDebug() << "inside void serialInterfaceIsDisconnected()" << endl;
#endif
    ui->connectionStatus->setText("- X -");
    ui->pushButton_SerialConnect->setText("Connect");
    ui->pushButton_SerialConnect->setEnabled(true);
    HiddenOutUi(true);
    emit sigSerialInterfaceClosureRequest();
#ifdef qqdebug
    qDebug() << "emit sigSerialInterfaceClosureRequest()" << endl;
#endif
}

void MainWindow::serialInterfaceInvalid()
{
    QMessageBox::critical(this, "Error", "Serial Interface is not working !!!");
    serialInterfaceIsDisconnected();
}

void MainWindow::displayMessageSent(const QByteArray &MessageSent)
{
    ToggleDisplayHighlightSendRead(1);
    NowUHV4 = &WindowCommand::WC(MessageSent);
    ui->label_MsgSnt->setText(QString(MessageSent.toHex()));
}

void MainWindow::displayMessageRead(const QByteArray &MessageRead)
{
    ToggleDisplayHighlightSendRead(2);
    NowUHV4 = &WindowCommand::WC(MessageRead);
    ui->label_MsgRd->setText(QString(MessageRead.toHex()));
}

void MainWindow::displayBufferCount(int Count)
{
#ifdef qqdebug
    qDebug() << "inside void displayBufferCount()" << endl;
#endif
    ui->label_20->setText(QString::number(Count));
}

void MainWindow::listSerialPortsToComboBox()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBoxSerial->addItem(info.portName());
}

void MainWindow::on_pushButton_SerialConnect_clicked()
{
    if (ui->pushButton_SerialConnect->text() == "Connect")
    {
#ifdef qqdebug
        qDebug() << "Button Connect Is Clicked !" << endl;
#endif
        serialInterfaceIsRequested();
    }
    else if (ui->pushButton_SerialConnect->text() == "Disconnect")
    {
#ifdef qqdebug
        qDebug() << "Button Disconnect Is Clicked !" << endl;
#endif
        serialInterfaceIsDisconnected();
    }
}

void MainWindow::on_pushButton_clicked()
{
    emit sigSerialInterfaceBufferClear();
}

void MainWindow::on_pushButton_7_clicked()
{
    if (ui->pushButton_7->text() == "Run Test")
        AutoSendnRead();
    else if (ui->pushButton_7->text() == "Pause Test")
        PauseAutoSendnRead();
}

void MainWindow::on_comboBoxSerial_currentIndexChanged(const QString &arg1)
{
    if (arg1 != "")
        serialInterfaceIsDisconnected();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->pushButton_2->text() == "HV ON")
    {
        ui->pushButton_2->setText("HV OFF");
        emit sigSerialInterfaceAddAPairToCommandList(APairOfPrioAndCommand(quint8(ui->spinBox->value()),NowUHV4->HVSwitch().On().GenerateMSG()));
#ifdef qqdebug
        qDebug() << "emit sigSerialInterfaceAddAPairToCommandList(HV ON)" << endl;
#endif
    }
    else
    {
        ui->pushButton_2->setText("HV ON");
        emit sigSerialInterfaceAddAPairToCommandList(APairOfPrioAndCommand(quint8(ui->spinBox->value()),NowUHV4->HVSwitch().Off().GenerateMSG()));
#ifdef qqdebug
        qDebug() << "emit sigSerialInterfaceAddAPairToCommandList(HV OFF)" << endl;
#endif
    }
}

void MainWindow::on_spinBox_9_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_10_valueChanged(int arg1)
{
    NowUHV4->SetChannel(arg1);
}

void MainWindow::on_pushButton_6_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
    AListOfPrioAndCommand tmpQMulMap;
    quint8 tmpKey = ui->spinBox_3->value();
    QByteArray tmpVal = NowUHV4->ReadP().GenerateMSG();
    for (int i = 1; i <= ui->spinBox_2->value(); i++)
        tmpQMulMap.insert(tmpKey, tmpVal);
    emit sigSerialInterfaceAddAListToCommandList(tmpQMulMap);
}

void MainWindow::on_pushButton_4_clicked()
{
    AListOfPrioAndCommand tmpQMulMap;
    quint8 tmpKey = ui->spinBox_5->value();
    QByteArray tmpVal = NowUHV4->ReadV().GenerateMSG();
    for (int i = 1; i <= ui->spinBox_4->value(); i++)
        tmpQMulMap.insert(tmpKey, tmpVal);
    emit sigSerialInterfaceAddAListToCommandList(tmpQMulMap);
}

void MainWindow::on_pushButton_5_clicked()
{
    AListOfPrioAndCommand tmpQMulMap;
    quint8 tmpKey = ui->spinBox_7->value();
    QByteArray tmpVal = NowUHV4->ReadI().GenerateMSG();
    for (int i = 1; i <= ui->spinBox_6->value(); i++)
        tmpQMulMap.insert(tmpKey, tmpVal);
    emit sigSerialInterfaceAddAListToCommandList(tmpQMulMap);
}

void MainWindow::ToggleDisplayHighlightSendRead(quint8 option)
{
    switch (option) {
    case 0:
        ui->label_MessageSent->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 0);}"));
        ui->label_MessageRead->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 0);}"));
        break;
    case 1:
        ui->label_MessageSent->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 255);}"));
        ui->label_MessageRead->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 0);}"));
        break;
    case 2:
        ui->label_MessageSent->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 0);}"));
        ui->label_MessageRead->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 0, 255);}"));
        break;
    default:
        break;
    }
}

