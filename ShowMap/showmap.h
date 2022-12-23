#ifndef SHOWMAP_H
#define SHOWMAP_H

#include <QWebEngineView>
#include <QWidget>

namespace Ui {
class ShowMap;
}

class ShowMap : public QWidget {
    Q_OBJECT

public:
    explicit ShowMap(QWidget* parent = nullptr);
    ~ShowMap();
public slots:
    // 清空地图上的标记点
    void slot_clear_car_position();
    // 设置地图上的标记点
    void slot_update_car_position(QString* glat, QString* glng, QString* image, QString* direction, QString* text);
private slots:
    void slot_page_load_finished(bool bIsFinished); //地图加载完毕槽函数
    void slot_page_load_progress(int progress);
    void slot_map_clickked(); //点击地图时槽函数
private:
    void InitMap();
    void InitData();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    static QString s_html_dir;

private:
    Ui::ShowMap* ui;
    QWebEngineView* m_web_engine_view;
};

#endif // SHOWMAP_H
