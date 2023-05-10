#include "spinboxdelegate.h"

SpinBoxDelegate::SpinBoxDelegate(bool i,QObject *parent)
       : QItemDelegate(parent),
         _i(i)
{
}
QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSpinBox *editor = new QSpinBox(parent);
    if(_i){
    editor->setMinimum(0);
    editor->setMaximum(65535);
    editor->setMaximumWidth(80);
    }else{
        editor->setMinimum(0);
        editor->setMaximum(255);
        editor->setMaximumWidth(50);
    }

    editor->setAlignment(Qt::AlignCenter);
    editor->installEventFilter(const_cast<SpinBoxDelegate*>(this));
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column()==1) {
        int r = model->index(index.row(),2).data(0).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        if(value>r){
            model->setData(index, r);
            return;
        }else{
            model->setData(index, value);
            return;
        }
    }
    if(index.column()==2){
        int r = model->index(index.row(),1).data(0).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        if(value<r){
            model->setData(index, r);
            return;
        }else{
            model->setData(index, value);
            return;
        }
    }
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
