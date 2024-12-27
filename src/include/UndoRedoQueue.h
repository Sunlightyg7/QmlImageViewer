#ifndef UNDOREDOQUEUE_H
#define UNDOREDOQUEUE_H

#include <deque>
#include <memory>
#include <QVariant>
#include <QString>
#include <QMap>

class State {
public:
	// 构造函数
	State() = default;
	State(const QMap<QString, QVariant>& elements) : elements(elements) {}

	// 拷贝构造函数
	State(const State& other) : elements(other.elements) {};

	const QMap<QString, QVariant>& getElements() const { return elements; }

private:
	// 每个元素是一个包含多个不同名称和值的元素，这里用 map 来表示
	QMap<QString, QVariant> elements;
};

class UndoRedoQueue
{
public:
	UndoRedoQueue();

	void push(const QMap<QString, QVariant>& newElements);

	// 撤销操作
	bool undo();

	// 重做操作
	bool redo();

	std::shared_ptr<State> getUndoState(int nIndex) const;

	/**
	 * @brief 合并撤销栈中同样名称的修改，只保留最新的修改
	 */
	QMap<QString, QVariant> incorpUndoStack();

	int size() const;

	void clear();

private:
	// 撤销栈，存储所有历史状态
	std::deque<std::shared_ptr<State>> undoStack;

	// 重做栈，存储撤销的状态
	std::deque<std::shared_ptr<State>> redoStack;
};
#endif
