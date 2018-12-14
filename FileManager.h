#pragma once

#include <string>
#include <vector>
#include <map>

typedef struct TrackFrameData {
	float x;
	float y;
	float code;
} TrackFrameData;

typedef std::map<unsigned int, TrackFrameData> TrackData;

class FileManager
{
private:
	static FileManager* m_instance;
	std::map<unsigned int, TrackData>  m_trackData;
	unsigned int boid_count;

public:
	FileManager();
	~FileManager();

	bool LoadTrackFile(std::string file_name);

	static FileManager* getInstance() {
		if (m_instance == nullptr) {
			m_instance = new FileManager();
		}
		return m_instance;
	}

	TrackFrameData GetTrackData(unsigned int no, unsigned int frame) {
		return m_trackData[no][frame];
	}
};

