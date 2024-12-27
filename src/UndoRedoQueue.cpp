#include "UndoRedoQueue.h"
#include <iostream>

using namespace std;

UndoRedoQueue::UndoRedoQueue()
{
}

void UndoRedoQueue::push(const QMap<QString, QVariant>& newElements)
{
	// 保存当前状态到撤销栈
	undoStack.push_back(std::make_shared<State>(newElements));
	// 清空重做栈，因为新的操作使得重做无效
	redoStack.clear();
}

bool UndoRedoQueue::undo()
{
	if (undoStack.empty()) {
		std::cout << "No operation to undo.\n";
		return false;
	}

	// 从撤销栈取出新的状态并更新
	shared_ptr<State> currentState = undoStack.back();
	undoStack.pop_back();

	// 将当前状态推入重做栈
	redoStack.push_back(currentState);

	return true;
}

bool UndoRedoQueue::redo()
{
	if (redoStack.empty()) {
		std::cout << "No operation to redo.\n";
		return false;
	}

	// 从重做栈取出新的状态并更新
	shared_ptr<State> currentState = redoStack.back();
	redoStack.pop_back();

	// 将当前状态推入撤销栈
	undoStack.push_back(currentState);

	return true;
}

std::shared_ptr<State> UndoRedoQueue::getUndoState(int nIndex) const
{
	if (nIndex >= 0 && nIndex < undoStack.size()) 
		return undoStack[nIndex];
	
	return nullptr;
}

QMap<QString, QVariant> UndoRedoQueue::incorpUndoStack()
{
	QMap<QString, QVariant> mTmpConfig;

	for (int i = 0; i < undoStack.size(); i++)
	{
		const QMap<QString, QVariant>& mElements = undoStack[i]->getElements();
		for(auto it = mElements.begin(); it != mElements.end(); ++it)
		{
			mTmpConfig.insert(it.key(), it.value());
		}
	}

	return mTmpConfig;
}

int UndoRedoQueue::size() const
{
	return undoStack.size();
}

void UndoRedoQueue::clear()
{
	undoStack.clear(); 
	redoStack.clear();
}