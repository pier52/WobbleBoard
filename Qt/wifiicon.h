/**
 * \class WiFiIcon
 *
 *
 * \brief
 *      Cutsom widget which displays a WiFi icon, and fills it depending on signal RSSI
 *
 * The wifi icon consists of 3 bars and 1 dot. By utilizing the slot <display> the rssi value passed
 * to it gets converted to a number of bars.
 *
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef WIFIICON_H
#define WIFIICON_H

#include <QWidget>
#include <QPainter>

class WiFiIcon : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief WiFiIcon
     * Constructor
     * @param parent
     * @param fill_color
     * The color the bars are filled with
     * @param background_color
     * The background color of the bars
     * @param contour_color
     * The contour color of the bars
     */
    explicit WiFiIcon(QWidget *parent = 0,QColor fill_color = Qt::blue ,QColor background_color = Qt::white, QColor contour_color = Qt::black);
    ~WiFiIcon();

signals:

public slots:

    /**
     * @brief display
     * Displays signal strength in the icon.
     *
     * @param RSSI
     * The RSSI signal strength in dbm
     * The following rules are applied (with deadbands, to avoid fluctuations) \n
     * RSSI > -50: All bars \n
     * RSSI > -67 : 3 Bars \n
     * RSSI > -70 : 2 Bars \n
     * RSSI > -80 : 1 Bar \n
     * RSSI < -80 : 0 Bars \n
     */
    void display(double RSSI);

    /**
     * @brief displayLevel
     * Displays signal strength in the icon.
     *
     * @param level
     * The signal strength in [0,100]
     * The following rules are applied (with deadbands, to avoid fluctuations) \n
     * level > 90: All bars \n
     * level > 70 : 3 Bars \n
     * level > 50 : 2 Bars \n
     * level > 30 : 1 Bar \n
     * level < 20 : 0 Bars \n
     */
    void displayLevel(int level);

    /// Clears all the bars
    void clearBars();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void drawWifiIcon(QPainter* painter);

    int number_of_bars = 0;
    QPen fill_pen;
    QPen background_pen;
    QPen contour_pen;

};

#endif // WIFIICON_H
