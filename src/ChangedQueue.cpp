#include "ChangedQueue.h"
#include <algorithm>

using namespace std;

ChangedQueue::ChangedQueue(QObject* parent) : QObject(parent)
{
}

void ChangedQueue::addOrUpdateStep(const Step& step)
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&step](const Step& s)
		{
			return s.szFuncName == step.szFuncName;
		});

	if (it != m_listSteps.end())
	{
		it->varValue = step.varValue;
		it->nPriority = step.nPriority;
	}
	else
	{
		m_listSteps.push_back(step);
	}

	emit stepsChanged();
}

void ChangedQueue::removeStep(const QString& szFuncName)
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szFuncName](const Step& s)
		{
			return s.szFuncName == szFuncName;
		});

	if (it != m_listSteps.end())
	{
		m_listSteps.erase(it);
		emit stepsChanged();
	}
}

void ChangedQueue::moveStep(int nFromIndex, int nToIndex)
{
	if (nFromIndex < 0 || nFromIndex >= m_listSteps.size() || nToIndex < 0 || nToIndex >= m_listSteps.size())
		return;

	Step step = m_listSteps[nFromIndex];
	m_listSteps.erase(m_listSteps.begin() + nFromIndex);
	m_listSteps.insert(m_listSteps.begin() + nToIndex, step);
	emit stepsChanged();
}

void ChangedQueue::adjustPriority(const QString& szFuncName, int nNewPriority)
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szFuncName](const Step& s)
		{
			return s.szFuncName == szFuncName;
		});

	if (it != m_listSteps.end())
	{
		it->nPriority = nNewPriority;
		std::sort(m_listSteps.begin(), m_listSteps.end(),
			[](const Step& a, const Step& b) -> bool
			{
				return a.nPriority < b.nPriority;
			});

		emit stepsChanged();
	}
}

Step ChangedQueue::getStep(const QString& szFuncName) const
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szFuncName](const Step& s)
		{
			return s.szFuncName == szFuncName;
		});

	if (it != m_listSteps.end())
		return *it;

	return Step();
}

QVariantList ChangedQueue::getStepsList() const
{
	QVariantList qmlSteps;
	for (const Step& step : m_listSteps) {
		// 将 Step 转换为 QVariantMap
		QVariantMap stepMap;
		stepMap["szFuncName"] = step.szFuncName;
		stepMap["varValue"] = step.varValue;
		stepMap["nPriority"] = step.nPriority;
		qmlSteps.append(stepMap);
	}
	return qmlSteps;
}

