#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QSerialPort>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void update_values(QString command);

    void readSerial();

    void update_gui(QString sensor_value);

private:
    Ui::Dialog *ui;
    QSerialPort *serialport;
    static const quint16 arduino_vendor_id=10755;
    static const quint16 arduino_product_id=67;
    QString arduino_port_name;
    bool arduino_available;
    QByteArray serialData;
    QString serialBuffer;
};

#endif // DIALOG_H
