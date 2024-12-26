#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <opencv2/opencv.hpp>
#include <deque>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class SnapshotManager
{
public:
	SnapshotManager(cv::ImwriteFlags eFlags = cv::IMWRITE_PNG_COMPRESSION, int nQuality = 9);
	~SnapshotManager() = default;

	/**
	 * @brief 保存图像快照为压缩文件
	 * @param [in] imgMat 要保存的图像
	 * @param [in] strOperation 操作名称
	 * @param [in] strFilePath 要保存的文件路径
	 */
	void saveSnapshot(const cv::Mat& imgMat, const std::string strOperation,const std::string& strFilePath);

	/**
	 * @brief 撤销.
	 * @param [out] currImgMat 上一次的图像
	 */
	void undo(cv::Mat& currImgMat);

	/**
	 * @brief 重做
	 * @param [out] currImgMat 执行撤销操作后，下一次的图像
	 */
	void redo(cv::Mat& currImgMat);

private:
	// 从磁盘加载快照
	cv::Mat loadSnapshot(const fs::path& pathFile);

	void clearRedoStack();

	const int m_nMaxUndo = 5;
	std::deque<std::pair<std::string, fs::path>> m_queUndoStack;  // 仅存储图像文件路径
	std::deque<std::pair<std::string, fs::path>> m_queRedoStack;
	std::vector<int> m_vCompressParams;
};
#endif
