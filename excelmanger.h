#ifndef EXCELMANGER_H
#define EXCELMANGER_H

#include <QAxObject>
#include <QMainWindow>

class ExcelManger : public QMainWindow {
    Q_OBJECT
  public:
    explicit ExcelManger(QWidget *parent = nullptr);

    void input_sid(QString SID);

    int input_check(QString SID);
  signals:

  public slots:

  private:
    QString filepath;

    QAxObject *excel;

    QAxObject *workbooks;
    QAxObject *workbook;
    QAxObject *worksheets; //获取工作表集合
    QAxObject *worksheet;  //获取工作表集合的工作表1，即sheet1

    bool isFileExist(QString fullFileName);

    QList<QList<QVariant>> data;
    void construct();
    void destory();

    /// @brief 创建一个Microsoft Excel文件
    bool create();
    /// @brief 打开一个Microsoft Excel文件
    bool open(const QString &filename = QString());

    QVariant readAll();
    void readAll(QList<QList<QVariant>> &cells);

    void writeAll(QList<QList<QVariant>> &cells);

    static void
        castListListVariant2Variant(const QList<QList<QVariant>> &cells, QVariant &res);
    static void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant>> &res);

    bool writeCurrentSheet(const QList<QList<QVariant>> &cells);
    static void convertToColName(int data, QString &res);
    static QString to26AlphabetString(int data);
};

#endif // EXCELMANGER_H
