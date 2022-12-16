#include "showmap.h"
#include "ui_showmap.h"
#include <QNetworkProxyFactory>
#include <QWebEngineSettings>

ShowMap::ShowMap(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ShowMap)
    , m_web_engine_view(nullptr)
{
    ui->setupUi(this);

    InitMap();
    InitData();
}

ShowMap::~ShowMap()
{
    qDebug("%s\n", __FUNCTION__);
    QString strCall = "destoryMap";
    m_web_engine_view->page()->runJavaScript(strCall);
    m_web_engine_view->deleteLater();
    delete ui;
}

void ShowMap::slot_update_car_position(QString* glat, QString* glng, QString* image, QString* direction, QString* text)
{
    QString strCall = QString("showCarPosition(\"%1\",\"%2\",\"%3\",\"%4\",\"%5\")").arg(*glng).arg(*glat).arg(*image).arg(*direction).arg(*text);
    qDebug("%s,str:%s\n", __FUNCTION__, strCall.toStdString().c_str());
    m_web_engine_view->page()->runJavaScript(strCall);
    delete glat;
    delete glng;
    delete image;
    delete direction;
    delete text;
}

void ShowMap::slot_page_load_finished(bool bIsFinished)
{
    qDebug() << "slotPageLoadFinished====================================" << bIsFinished;

    // 默认襄阳的位置
    // QString lng = "112.13555";
    // QString lat = "32.04487";
    // QString strCall = "setCenter(" + lng + "," + lat + ")";
    // m_web_engine_view->page()->runJavaScript(strCall);
}

void ShowMap::slot_page_load_progress(int progress)
{
    qDebug() << __FUNCTION__ << ",progress=" << progress << "\n";
}

void ShowMap::slot_map_clickked()
{
    qDebug() << __FUNCTION__ << "\n";
}
void ShowMap::InitMap()
{
    // https://www.itdaan.com/blog/2016/10/22/48e4db27f97f.html
    // Debug模式下，地图加载会很慢，使用release版本，速度就会很快。
    m_web_engine_view = new QWebEngineView(this);
    m_web_engine_view->setGeometry(this->rect());

    connect(m_web_engine_view, SIGNAL(loadFinished(bool)), this, SLOT(slot_page_load_finished(bool)));
    connect(m_web_engine_view, SIGNAL(loadProgress(int)), this, SLOT(slot_page_load_progress(int)));

    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    m_web_engine_view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
}

void ShowMap::InitData()
{
    m_web_engine_view->load(QUrl("E:/code/github/video_pc_tmp/ShowMap/real_video_map_gaode.html"));
}

// 窗口变动时，也更新地图展示
void ShowMap::resizeEvent(QResizeEvent* event)
{
    m_web_engine_view->setGeometry(this->rect());
}
