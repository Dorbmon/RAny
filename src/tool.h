#pragma once
#include <array>
#include <iostream>
#include <string>
#include <filesystem>
static std::string generateRandomString(int length) {
    const std::array characters = std::to_array("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const int charactersLength = characters.size();
    
    std::string randomString;
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < length; ++i) {
        randomString += characters[rand() % charactersLength];
    }

    return randomString;
}
static size_t getFileSize(const char *fileName) {
	if (fileName == NULL) {
		return 0;
	}
    std::filesystem::path path(fileName);

    return std::filesystem::file_size(path);
}