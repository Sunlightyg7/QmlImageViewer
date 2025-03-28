#ifndef CHANGEDQUEUE_H
#define CHANGEDQUEUE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QObject>

struct Step 
{
    QString szVariableName; // 变量名如：gamma、brightness
    QVariant varValue;
    unsigned short nPriority; 
    bool bSelected;
};

class ChangedQueue : public QObject
{
    Q_OBJECT
public:
    explicit ChangedQueue(QObject* parent = nullptr);

    void addOrUpdateStep(const Step& step);
    bool removeStep(const QString& szVariableName);
    bool removeStep(int nIndex);
    // 移动调整步骤的位置
    bool moveStep(int nFromIndex, int nToIndex);
    bool adjustPriority(const QString& szVariableName, int nNewPriority);
    bool stepSelected(int nIndex);
    Step getStep(const QString& szVariableName) const;
    Step getStep(int nIndex) const;
    QList<Step> getSteps() const;
    QVariantList getStepsList() const;
    int size() const;
    void clear();
    QList<Step>::const_iterator begin() const;
    QList<Step>::const_iterator end() const;

signals:
    void stepsChanged();

private:
    QList<Step> m_listSteps;
};
#endif 
