#include "onyx/data/handwriting_functions_model.h"
#include "onyx/data/handwriting_interface.h"

namespace handwriting
{

static const int NAME_ROLE = Qt::UserRole + 1;
static const int FREQUENCY_ROLE = Qt::UserRole + 2;

static QString getDisplayNameByLocale(int locale)
{
    QString display_name;
    QLocale::Language language = static_cast<QLocale::Language>(locale);
    switch (language)
    {
    case QLocale::English:
        display_name = QApplication::tr("English");
        break;
    case QLocale::Chinese:
        display_name = QApplication::tr("Chinese");
        break;
    case QLocale::Russian:
        display_name = QApplication::tr("Russian");
        break;
    case QLocale::Japanese:
        display_name = QApplication::tr("Japanese");
        break;
    case QLocale::Dutch:
        display_name = QApplication::tr("Dutch");
        break;
    default:
        break;
    }
    return display_name;
}

HandwritingFunctionsModel::HandwritingFunctionsModel()
{
}

HandwritingFunctionsModel::~HandwritingFunctionsModel()
{
}

void HandwritingFunctionsModel::getModel(QStandardItemModel & functions_model)
{
    QVector<int> items = getDefaultSettings();
    functions_model.setColumnCount(1);
    functions_model.setSortRole(FREQUENCY_ROLE);
    QVector<int>::iterator begin = items.begin();
    QVector<int>::iterator end   = items.end();
    int row = 0;
    for(QVector<int>::iterator iter  = begin; iter != end; ++iter, ++row)
    {
        int item = (*iter);
        QString display_name;
        if (item > 0)
        {
            display_name = getDisplayNameByLocale(item);
        }
        else
        {
            if (item == NUMBER)
            {
                display_name = QApplication::tr("Number");
            }
            else if (item == INTERPUNCTION)
            {
                display_name = QApplication::tr("Interpunction");
            }
        }

        QStandardItem *model_item = new QStandardItem(display_name);
        model_item->setEditable(false);
        functions_model.setItem(row, 0, model_item);

        // use frequency
        QSettings settings;
        QString item_str("%1");
        settings.beginGroup("Handwriting/" + item_str.arg(item));
        bool ok = false;
        int frequency = settings.value("frequency").toInt(&ok);
        if (!ok)
        {
            frequency = 0;
        }
        model_item->setData(frequency, FREQUENCY_ROLE);
        model_item->setData(item, NAME_ROLE);
        settings.endGroup();
    }

    sort(functions_model);
}

QVector<int> HandwritingFunctionsModel::getDefaultSettings()
{
    static QVector<int> settings;
    if (settings.isEmpty())
    {
        settings.push_back(QLocale::English);
        settings.push_back(QLocale::Chinese);
        settings.push_back(-1);         // symbol
        settings.push_back(-2);         // number
        settings.push_back(QLocale::Russian);
        settings.push_back(QLocale::Japanese);
        settings.push_back(QLocale::Dutch);
    }
    return settings;
}

void HandwritingFunctionsModel::sort(QStandardItemModel & functions_model)
{
    QStandardItem * root = functions_model.invisibleRootItem();
    root->sortChildren(0, Qt::DescendingOrder);
}

void HandwritingFunctionsModel::onItemClicked(const QModelIndex & index, QStandardItemModel & functions_model)
{
    int frequency = index.data(FREQUENCY_ROLE).toInt() + 1;
    int item = index.data(NAME_ROLE).toInt();

    // update functions order
    QStandardItem * model_item = functions_model.itemFromIndex(index);
    model_item->setData(frequency, FREQUENCY_ROLE);
    sort(functions_model);

    // save in settings
    QSettings settings;
    QString item_str("%1");
    settings.beginGroup("Handwriting/" + item_str.arg(item));
    settings.setValue("frequency", frequency);
    settings.endGroup();
}

QModelIndex HandwritingFunctionsModel::getIndexBySetting(int setting, QStandardItemModel & functions_model)
{
    for (int i = 0; i < functions_model.rowCount(); ++i)
    {
        QStandardItem * item = functions_model.item(i);
        int item_name = item->data(NAME_ROLE).toInt();
        if (item_name == setting)
        {
            return item->index();
        }
    }
    return QModelIndex();
}

}
