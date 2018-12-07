#include "excelmanger.h"
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
//#include <QVariant>

#define TC_FREE(x) \
    {              \
        delete x;  \
        x = NULL;  \
    }

ExcelManger::ExcelManger(QWidget *parent)
    : QMainWindow(parent)
    , excel(nullptr)
    , workbooks(nullptr)
    , workbook(nullptr)
    , worksheets(nullptr)
    , worksheet(nullptr) {
    filepath = QDir::currentPath();

    filepath += tr("/log/record.xlsx");
    qDebug() << filepath;
    if (!isFileExist(filepath)) {
        qDebug() << tr("creat new file");
        create();
        workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filepath)); //保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
    } else {
        qDebug() << tr("open file");
        open(filepath);
    }

    readAll(data);

    destory();
}

void ExcelManger::input_sid(QString deviceName, QString productKey, QString deviceSecret) {
    QList<QVariant> var;
    var.append(deviceName);
    var.append(productKey);
    var.append(deviceSecret);
    data.append(var);
    writeAll(data);
}

int ExcelManger::input_check(QString deviceName) {
    int row = data.size();
    for (int i = 0; i < row; i++) {
        qDebug() << data.at(i).at(0).toString() + tr(":") + deviceName;
        if (QString::compare(deviceName, data.at(i).at(0).toString(), Qt::CaseSensitive) == 0)
            return 0;
    }
    return 1;
}

void ExcelManger::output_sid(QString deviceName) {
    int row = data.size();
    for (int i = 0; i < row; i++) {
        qDebug() << data.at(i).at(0).toString() + tr(":") + deviceName;
        if (QString::compare(deviceName, data.at(i).at(0).toString(), Qt::CaseSensitive) == 0) {

            QVariantHash root;
            root.insert("code", 100);
            QVariantHash sid_data;
            sid_data.insert("deviceName", data.at(i).at(0).toString());
            sid_data.insert("productKey", data.at(i).at(1).toString());
            sid_data.insert("deviceSecret", data.at(i).at(2).toString());
            root.insert("data", sid_data);
            root.insert("message", "success");
            QJsonObject rootObj = QJsonObject::fromVariantHash(root);
            QJsonDocument json;
            json.setObject(rootObj);
            qDebug() << "json" << json.toJson();
            emit json_back(json);
            break;
        }
    }
}

bool ExcelManger::isFileExist(QString fullFileName) {
    QFileInfo fileInfo(fullFileName);
    if (fileInfo.isFile()) {
        return true;
    }
    return false;
}

void ExcelManger::construct() {
    destory();
    excel = new QAxObject("Excel.Application");
    excel->setProperty("Visible", false);
    excel->setProperty("DisplayAlerts", false);
    if (excel->isNull()) {
        excel->setControl("ET.Application");
    }
    if (!excel->isNull()) {
        workbooks = excel->querySubObject("Workbooks");
    }
}

void ExcelManger::destory() {
    if (workbook != nullptr && !workbook->isNull()) {
        workbook->dynamicCall("Close()");
    }

    if (excel != nullptr && !excel->isNull()) {
        excel->dynamicCall("Quit()");
    }

    TC_FREE(worksheet);
    TC_FREE(worksheets);
    TC_FREE(workbook);
    TC_FREE(workbooks);
    TC_FREE(excel);
}

bool ExcelManger::create() {
    bool ret = false;
    construct();
    if (workbooks != nullptr && !workbooks->isNull()) {
        workbooks->dynamicCall("Add");
        workbook = excel->querySubObject("ActiveWorkBook");
        worksheets = workbook->querySubObject("Sheets");
        worksheet = worksheets->querySubObject("Item(int)", 1); //获取工作表集合的工作表1，即sheet1
        QAxObject *cellA, *cellB, *cellC;

        //设置标题
        int cellrow = 1;
        QString A = "A" + QString::number(cellrow); //设置要操作的单元格，如A1
        QString B = "B" + QString::number(cellrow);
        QString C = "C" + QString::number(cellrow);

        cellA = worksheet->querySubObject("Range(QVariant, QVariant)", A); //获取单元格
        cellB = worksheet->querySubObject("Range(QVariant, QVariant)", B);
        cellC = worksheet->querySubObject("Range(QVariant, QVariant)", C);

        cellA->dynamicCall("SetValue(const QVariant&)", QVariant("DeviceName")); //设置单元格的值
        cellB->dynamicCall("SetValue(const QVariant&)", QVariant("PoduceKey"));
        cellC->dynamicCall("SetValue(const QVariant&)", QVariant("DeviceSecret"));

        ret = true;
    }
    return ret;
}

bool ExcelManger::open(const QString &filename) {
    bool ret = false;
    construct();
    if (workbooks != nullptr && !workbooks->isNull()) {
        workbook = workbooks->querySubObject("Open(QString&", QDir::toNativeSeparators(filename));
        ret = workbook != nullptr && !workbook->isNull();
        if (ret) {
            worksheets = workbook->querySubObject("Sheets");        //获取工作表集合
            worksheet = worksheets->querySubObject("Item(int)", 1); //获取工作表集合的工作表1，即sheet1
        } else {
            TC_FREE(workbook);
        }
    }
    return ret;
}

QVariant ExcelManger::readAll() {
    QVariant var;
    if (worksheet != nullptr && !worksheet->isNull()) {
        QAxObject *usedRange = worksheet->querySubObject("UsedRange");
        if (nullptr == usedRange || usedRange->isNull()) {
            return var;
        }
        var = usedRange->dynamicCall("Value");
        delete usedRange;
    }
    return var;
}

void ExcelManger::readAll(QList<QList<QVariant>> &cells) {
    castVariant2ListListVariant(readAll(), cells);
}

void ExcelManger::writeAll(QList<QList<QVariant>> &cells) {
    if (open(filepath) != false) {
        writeCurrentSheet(cells);
        workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filepath)); //保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
        destory();
    }
}

void ExcelManger::castListListVariant2Variant(const QList<QList<QVariant>> &cells, QVariant &res) {
    QVariantList vars;
    const int rows = cells.size();
    for (int i = 0; i < rows; ++i) {
        vars.append(QVariant(cells[i]));
    }
    res = QVariant(vars);
}

void ExcelManger::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant>> &res) {

    QVariantList varRows = var.toList();
    if (varRows.isEmpty()) {
        return;
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    for (int i = 0; i < rowCount; ++i) {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
}

bool ExcelManger::writeCurrentSheet(const QList<QList<QVariant>> &cells) {
    if (cells.size() <= 0)
        return false;
    if (nullptr == worksheet || worksheet->isNull())
        return false;
    int row = cells.size();
    int col = cells.at(0).size();
    QString rangStr;
    convertToColName(col, rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;
    qDebug() << rangStr;
    QAxObject *range = worksheet->querySubObject("Range(const QString&)", rangStr);
    if (nullptr == range || range->isNull()) {
        return false;
    }
    bool succ = false;
    QVariant var;
    castListListVariant2Variant(cells, var);
    succ = range->setProperty("Value", var);
    delete range;
    return succ;
}

void ExcelManger::convertToColName(int data, QString &res) {
    Q_ASSERT(data > 0 && data < 65535);
    int tempData = data / 26;
    if (tempData > 0) {
        int mode = data % 26;
        convertToColName(mode, res);
        convertToColName(tempData, res);
    } else {
        res = (to26AlphabetString(data) + res);
    }
}

QString ExcelManger::to26AlphabetString(int data) {
    QChar ch = data + 0x40; //A对应0x41
    return QString(ch);
}
