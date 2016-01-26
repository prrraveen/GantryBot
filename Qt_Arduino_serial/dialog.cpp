#include "dialog.h"
#include "ui_dialog.h"
#include<QtCore>
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QDebug>
#include<QMessageBox>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->temprature_lcd->display("-------");
    ui->light_intensity_lcd->display("------");
    arduino_available=false;
    arduino_port_name="";
    serialBuffer="";
    serialport=new QSerialPort;

    /*find which port is connected to arduino
    qDebug()<<"No. of ports "<<QSerialPortInfo::availablePorts().length();

    foreach (const QSerialPortInfo &serialportinfo, QSerialPortInfo::availablePorts()) {
        qDebug()<<"Port name : "<<serialportinfo.portName();

        qDebug()<<"Has Vendor ID: "<<serialportinfo.hasVendorIdentifier();  //vendor id is usb implement form, company's id
        if(serialportinfo.vendorIdentifier()){
            qDebug()<<"Vendor Id : "<<serialportinfo.vendorIdentifier();
        }

        qDebug()<<"Has Product ID: "<<serialportinfo.hasProductIdentifier();  //product id
        if(serialportinfo.productIdentifier()){
            qDebug()<<"Product Id : "<<serialportinfo.productIdentifier();
        }
    }
*/

    foreach (const QSerialPortInfo &serialportinfo, QSerialPortInfo::availablePorts()) {
        if(serialportinfo.hasVendorIdentifier() && serialportinfo.hasProductIdentifier()){
            if(serialportinfo.vendorIdentifier() == arduino_vendor_id){
                if(serialportinfo.productIdentifier() == arduino_product_id){
                    qDebug()<<"port name: "<<serialportinfo.portName();
                     arduino_port_name=serialportinfo.portName();
                     arduino_available=true;
                }
            }
        }
    }

    if(arduino_available){
        // open and configure the port
        qDebug()<<"Found Arduino:";
        serialport->setPortName(arduino_port_name);
        serialport->open(QSerialPort::ReadWrite);
        serialport->setBaudRate(QSerialPort::Baud9600);   //first open the port and then set all the parameter for serial port
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setParity(QSerialPort::NoParity);
        serialport->setStopBits(QSerialPort::OneStop);
        serialport->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(serialport,SIGNAL(readyRead()),this,SLOT(readSerial()));
    }else{
        //give error message
        QMessageBox::warning(this,"Port Error","Couldn't find Arduino!");
    }
}

Dialog::~Dialog()
{
    //check if serialport already open, close it
    if(serialport->isOpen()){
        serialport->close();
    }
    delete ui;
}

void Dialog::on_horizontalSlider_2_valueChanged(int value)
{
    ui->label_3->setText(QString("%1").arg(value));
    Dialog::update_values(QString("r%1").arg(value)); //transmitting values like r255 for red and g255 for green
}

void Dialog::on_horizontalSlider_valueChanged(int value)
{
    ui->label_4->setText(QString("%1").arg(value));
    Dialog::update_values(QString("g%1").arg(value));
}


//to write on serial port
void Dialog::update_values(QString command){
    if(serialport->isWritable()){
        serialport->write(command.toStdString().c_str());  //have to convert QString to std string and c string
        serialport->flush();
    }else{
        qDebug()<<"Couldn't write to serial port";
    }
}


//to read serial data
void Dialog::readSerial(){
    qDebug()<<"Reading Serial";
    QStringList bufferList=serialBuffer.split("\r\n");
    qDebug()<<bufferList;
    //first values comes in bufferlist will be random number which is not correct value, that should not show on gui
    if(bufferList.length()<3){
        serialData=serialport->readAll();
        serialBuffer +=QString::fromStdString(serialData.constData());
    }else{
        qDebug()<<"buffer value"<<bufferList;
        QString final_value=bufferList[1];  //sending data like "l:1023,t:32" from arduino,now split on the basis of t and l.
        Dialog::update_gui(final_value);
        serialBuffer="";
    }

}

void Dialog::update_gui(const QString sensor_value){
    QStringList comp_list=sensor_value.split(",");
    ui->light_intensity_lcd->display(comp_list[0].split(':')[1]);
    ui->temprature_lcd->display(comp_list[1].split(":")[1]);
}

