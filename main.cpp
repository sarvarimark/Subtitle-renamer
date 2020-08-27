#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <list>
#include <regex>

#define _USER_MSG

namespace fs = std::filesystem;

std::ifstream extSubtitle;
std::ifstream extVideo;
std::list<fs::directory_entry> toExplore;
std::list<fs::directory_entry> currentSubtitle;
std::list<fs::directory_entry> currentVideo;

void print_decoration()
{
	std::cout <<  "||\n||\n========================================================================================================================\n||\n||\n";
}

bool is_in(const fs::path ext, std::ifstream& file)
{
	std::string line;
	
	try
	{
		file.clear();
		file.seekg(0, std::ios::beg);
		if (!file.good()) { throw new std::exception; }

		while (getline(file, line))
		{
			if (line == ext.string())
			{
				return true;
			}
		}
		return false;
	}
	catch (std::exception&)
	{
		std::cout << "||\n||\tAn internal file error occured, please restart the program.\n||\n";
		return false;
	}
}

bool dir_less(fs::directory_entry lhs, fs::directory_entry rhs)
{
	return lhs.path().filename() < rhs.path().filename();
}

int main(int argc, char** argv)
{
	print_decoration();
	fs::directory_entry dir;
	switch (argc)
	{
		case 1:		std::cout << "||\tPlease input the directory in which you wish to syncronize the subtitles with videos."; return -1;
		case 2:		try { dir.assign(fs::path(argv[1])); }
					catch (const std::exception&) { std::cout << "||\tNon existing directory: " << argv[1]; return -2; }; break;
		default:	std::cout << "||\tToo many given arguments, please input only one directory at a time."; return -3;
	}
	
	extSubtitle.open("extensions\\extSubtitle.txt", std::ifstream::in);
	if (extSubtitle.fail())	std::cout << "||\tCould not open extSubtitle.txt, please check the file and restart.\n";
	extVideo.open("extensions\\extVideo.txt", std::ifstream::in);
	if (extVideo.fail())	std::cout << "||\tCould not open extVideo.txt, please check the file and restart.\n";

	toExplore.push_front(dir);

	while (!toExplore.empty())
	{
		for (const auto& current : fs::directory_iterator(toExplore.back()))
		{
			if (current.is_directory())
			{
				toExplore.push_front(current);
			}
			else if (current.path().has_extension())
			{
				if (is_in(current.path().extension(), extVideo))
				{
					currentVideo.push_front(current);
				}
				else if (is_in(current.path().extension(), extSubtitle))
				{
					currentSubtitle.push_front(current);
				}
			}
		}
		
		if ( (currentSubtitle.size() != 0) || (currentVideo.size() != 0) )
		{
			if (currentSubtitle.size() == currentVideo.size())
			{
				currentSubtitle.sort(dir_less);
				currentVideo.sort(dir_less);
				
				while (!currentSubtitle.empty())
				{
					fs::path newPath = currentVideo.back().path().parent_path() /= currentVideo.back().path().stem() += currentSubtitle.back().path().extension();
					try
					{
						rename(currentSubtitle.back(), newPath);
					}
					catch (std::exception&)
					{
						std::cout << "||\tAn error happened during renaming \"" << currentSubtitle.back().path() << "\" into \"" << newPath << "\"\n";
						return -5;
					}
#ifdef _USER_MSG
					std::cout << "||\tSuccessful rename from " << currentSubtitle.back().path().filename() << " to " << newPath.filename() << "\n";
#endif
					currentSubtitle.pop_back();
					currentVideo.pop_back();
				}
			}
			else if (false) //TODO recognize series
			{

			}
#ifdef _USER_MSG
			else
			{
				std::cout << "||\tThe number of subtitles (" << currentSubtitle.size() << ") and the number of videos (" << currentVideo.size() << ") is not equal in: " << toExplore.back().path() << "\n";
			}
#endif
		}	
		toExplore.pop_back();
		currentSubtitle.clear();
		currentVideo.clear();
	}
	extSubtitle.close();		//TODO: catch failbit, never happened
	extVideo.close();
	
	print_decoration();

	return 0;
}