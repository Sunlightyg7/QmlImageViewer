#include "SnapshotManager.h"
#include <future>
#include <iostream>

using namespace std;

SnapshotManager::SnapshotManager(cv::ImwriteFlags eFlags, int nQuality)
{
	// 设置压缩参数，PNG 格式
	m_vCompressParams.push_back(eFlags);
	m_vCompressParams.push_back(nQuality);  // 最大压缩
}

void SnapshotManager::saveSnapshot(const cv::Mat& imgMat, const std::string strOperation, const std::string& strFilePath)
{
	// 保存图像快照为压缩文件
	bool bSuccess = cv::imwrite(strFilePath, imgMat, m_vCompressParams);
	if (bSuccess)
	{
		std::cout << "图像快照已保存为 " << strFilePath << std::endl;

		// 在撤销栈中仅保留文件路径
		m_queUndoStack.push_back(make_pair(strOperation, strFilePath));

		if (m_queUndoStack.size() > m_nMaxUndo)
			m_queUndoStack.pop_front();

		clearRedoStack();  // 清空重做栈
	}
	else
		std::cerr << "图像保存失败！" << std::endl;
}

void SnapshotManager::undo(cv::Mat& currImgMat)
{
	if (m_queUndoStack.empty()) {
		std::cerr << "没有更多的撤销操作!" << std::endl;
		return;
	}

	// 获取并恢复上一个快照（从文件加载）
	auto&& pair = m_queUndoStack.back();
	m_queRedoStack.push_back(pair);
	m_queUndoStack.pop_back();

	currImgMat = loadSnapshot(pair.second);
}

void SnapshotManager::redo(cv::Mat& currImgMat)
{
	if (m_queRedoStack.empty()) {
		std::cerr << "没有更多的重做操作!" << std::endl;
		return;
	}

	// 获取并恢复下一个快照（从文件加载）
	auto&& pair = m_queRedoStack.back();
	m_queUndoStack.push_back(pair);
	m_queRedoStack.pop_back();

	currImgMat = loadSnapshot(pair.second);
}

cv::Mat SnapshotManager::loadSnapshot(const fs::path& pathFile)
{
	return cv::imread(pathFile.string(), cv::IMREAD_UNCHANGED);
}

void SnapshotManager::clearRedoStack()
{
	while (!m_queUndoStack.empty())
	{
		auto&& [strOperation, pathFile] = m_queUndoStack.back();
		m_queUndoStack.pop_back();

		// 检查文件是否存在
		if (fs::exists(pathFile)) {
			// 尝试删除文件
			try {
				if (fs::remove(pathFile)) {
					std::cout << "File: [" << pathFile << "] deleted successfully." << std::endl;
				}
				else {
					std::cout << "File: [" << pathFile << "] deletion failed." << std::endl;
				}
			}
			catch (const fs::filesystem_error& e) {
				std::cout << "Error deleting file: [" << pathFile << "] because: " << e.what() << std::endl;
			}
		}
		else {
			std::cout << "File: [" << pathFile << "] does not exist." << std::endl;
		}
	}
}
