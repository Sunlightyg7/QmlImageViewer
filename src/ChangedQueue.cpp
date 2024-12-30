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
			return s.szVariableName == step.szVariableName;
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

bool ChangedQueue::removeStep(const QString& szVariableName)
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szVariableName](const Step& s)
		{
			return s.szVariableName == szVariableName;
		});

	if (it != m_listSteps.end())
	{
		m_listSteps.erase(it);
		emit stepsChanged();
		return true;
	}

	return false;
}

bool ChangedQueue::removeStep(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_listSteps.size())
		return false;

	m_listSteps.erase(m_listSteps.begin() + nIndex);
	emit stepsChanged();
	return true;
}

bool ChangedQueue::moveStep(int nFromIndex, int nToIndex)
{
	if (nFromIndex < 0 || nFromIndex >= m_listSteps.size() || nToIndex < 0 || nToIndex >= m_listSteps.size())
		return false;

	Step step = m_listSteps[nFromIndex];
	m_listSteps.erase(m_listSteps.begin() + nFromIndex);
	m_listSteps.insert(m_listSteps.begin() + nToIndex, step);
	emit stepsChanged();
	return true;
}

bool ChangedQueue::adjustPriority(const QString& szVariableName, int nNewPriority)
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szVariableName](const Step& s)
		{
			return s.szVariableName == szVariableName;
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
		return true;
	}

	return false;
}

bool ChangedQueue::stepSelected(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_listSteps.size())
		return false;

	for (auto it = m_listSteps.begin(); it != m_listSteps.end(); it++)
	{
		it->bSelected = false;
	}

	m_listSteps[nIndex].bSelected = !m_listSteps[nIndex].bSelected;
	emit stepsChanged();
	return true;
}

Step ChangedQueue::getStep(const QString& szVariableName) const
{
	auto it = std::find_if(m_listSteps.begin(), m_listSteps.end(),
		[&szVariableName](const Step& s)
		{
			return s.szVariableName == szVariableName;
		});

	if (it != m_listSteps.end())
		return *it;

	return Step();
}

Step ChangedQueue::getStep(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_listSteps.size())
		return Step();

	return m_listSteps[nIndex];
}

QVariantList ChangedQueue::getStepsList() const
{
	QVariantList qmlSteps;
	for (const Step& step : m_listSteps) {
		// 将 Step 转换为 QVariantMap
		QVariantMap stepMap;
		stepMap["szVariableName"] = step.szVariableName;
		stepMap["varValue"] = step.varValue;
		stepMap["nPriority"] = step.nPriority;
		stepMap["bSelected"] = step.bSelected;
		qmlSteps.append(stepMap);
	}
	return qmlSteps;
}

