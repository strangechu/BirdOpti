#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

FileManager* FileManager::m_instance = nullptr;

FileManager::FileManager()
{
	boid_count = 0;
}


FileManager::~FileManager()
{
}

// Load track file into vector
bool FileManager::LoadTrackFile(std::string file_name) {
	std::ifstream in_file;
	in_file.open(file_name);
	if (!in_file) {
		std::cout << "Unable to open file " << file_name << std::endl;
		return false;
	}
	
	std::string line;
	std::string cell;
	int phase = 0;
	int frame = 0;

	TrackData track_data;

	// start reading
	while (std::getline(in_file, line)) {
		std::stringstream line_stream(line);
		std::getline(line_stream, cell, ',');
		if (cell.compare("x") == 0) {
			phase = 1;
			continue;
		}
		else if (cell.compare("ID") == 0) {
			phase = 2;
		}
		if (phase == 1) {
			TrackFrameData data;
			if (cell.size() == 0) {
				continue;
			}
			data.x = std::stof(cell);
			std::getline(line_stream, cell, ',');
			data.y = std::stof(cell);
			std::getline(line_stream, cell, ',');
			data.code = std::stoi(cell);

			track_data.insert(std::pair<unsigned int, TrackFrameData>(frame, data));

			frame++;
		}
	}

	m_trackData.insert(std::pair<unsigned int, TrackData>(boid_count, track_data));
	boid_count++;

	in_file.close();

	return true;
}