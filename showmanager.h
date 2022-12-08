#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include "Login/login.h"
#include "ShowMap/showmap.h"
#include "car_video_view/carvideoplayer.h"
#include <QJsonValue>
#include <QObject>
#include <QString>

class ShowManager : public QObject {
    Q_OBJECT
public:
    explicit ShowManager(QObject* parent = nullptr);
    ~ShowManager();
signals:
private slots:
    void slot_login_success(QString* user, QJsonValue* data);

public:
    void Start();

private:
    Login* m_login;
    CarVideoPlayer* m_car_video_player;
    ShowMap* m_show_map;
};

#endif // SHOWMANAGER_H
