#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "ui_dialog.h"

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // DIALOG_HPP
