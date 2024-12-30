#ifndef CHANGEDQUEUE_H
#define CHANGEDQUEUE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QObject>

struct Step 
{
    QString szFuncName;
    QVariant varValue;
    unsigned short nPriority; 
    bool bSelected;
};

class ChangedQueue : public QObject
{
    Q_OBJECT
public:
    explicit ChangedQueue(QObject* parent = nullptr);

    // 添加或更新调整步骤
    void addOrUpdateStep(const Step& step);

    // 删除调整步骤
    bool removeStep(const QString& szFuncName);

    bool removeStep(int nIndex);

    // 移动调整步骤的位置
    bool moveStep(int nFromIndex, int nToIndex);

    // 调整步骤的优先级
    bool adjustPriority(const QString& szFuncName, int nNewPriority);

    bool stepSelected(int nIndex);

    Step getStep(const QString& szFuncName) const;
    QList<Step> getSteps() const { return m_listSteps; }
    QVariantList getStepsList() const;
    int size () const { return m_listSteps.size(); }
    void clear() { m_listSteps.clear(); }
    auto begin() const { return m_listSteps.begin(); }
	auto end() const { return m_listSteps.end(); }


signals:
    void stepsChanged();

private:
    QList<Step> m_listSteps;
};
#endif 
