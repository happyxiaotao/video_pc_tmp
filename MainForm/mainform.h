#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

namespace Ui {
class MainForm;
}

class MainForm : public QWidget {
    Q_OBJECT

public:
    explicit MainForm(const QString& user);
    ~MainForm();
signals:
    void sig_quit_account();
private slots:
    void slot_quit_account();

private:
    Ui::MainForm* ui;

    QString m_user;
};

#endif // MAINFORM_H
