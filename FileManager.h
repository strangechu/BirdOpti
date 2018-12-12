#pragma once

#include <string>
#include <vector>

typedef struct TrackData {
	int no;
	float x;
	float y;
	float code;
} TrackData;

class FileManager
{
private:
	static FileManager* m_instance;
	std::vector<TrackData> m_trackData;

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
};

