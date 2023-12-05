#include <string>

bool convertToPCM(const std::string& inputFileName, const std::string& outputFileName, std::function<void(double)> update_progress_callback);