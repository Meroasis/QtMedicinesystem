#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <Qtsql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>
class IDatabase : public QObject
{
    Q_OBJECT
public:
    static IDatabase& getInstance()
    {
        static IDatabase instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
        return instance;
    }
    QString userLogin(QString username,QString password);
    bool initPatientModel();

    QSqlTableModel  *patientTabModel; //数据模型
    QItemSelectionModel *thePatientSelection;//选择模型
    QDataWidgetMapper  *dataMapper;//数据映射

    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void reverPatientEidt();
    int addNewPatient();


private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const&)  = delete;
    void operator=(IDatabase const&)  = delete;
    QSqlDatabase database;



    void initDatabase();


signals:


};

#endif // IDATABASE_H
