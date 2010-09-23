/**
*  utilite is a cross-platform library with
*  useful utilities for fast and small developing.
*  Copyright (C) 2010  Mathieu Labbe
*
*  utilite is free library: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  utilite is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "UDirectory.h"

#ifdef WIN32
  #include <Windows.h>
  #include <direct.h>
  #include <algorithm>
#else
  #include <dirent.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <sys/param.h>
  #include <sys/dir.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
#endif

#include "UStl.h"
#include "UFile.h"
#include "UDirectory.h"

#include "ULogger.h"

#ifdef WIN32

bool sortCallback(const std::string & a, const std::string & b)
{
	return uStrNumCmp(a,b) < 0;
}
#elif __APPLE__
int sortCallback(const void * aa, const void * bb)
{
	const struct dirent ** a = (const struct dirent **)aa;
	const struct dirent ** b = (const struct dirent **)bb;

	return uStrNumCmp((*a)->d_name, (*b)->d_name);
}
#else
int sortCallback( const dirent ** a,  const dirent ** b)
{
	return uStrNumCmp((*a)->d_name, (*b)->d_name);
}
#endif

UDirectory::UDirectory(const std::string & path, const std::string & extensions)
{
	_extensions = uListToVector(uSplit(extensions, ' '));
	_path = path;
	this->update();
	_iFileName = _fileNames.begin();
}

UDirectory::~UDirectory()
{
}

void UDirectory::update()
{
	if(exists(_path))
	{
#ifdef WIN32
		WIN32_FIND_DATA fileInformation;
		HANDLE hFile  = ::FindFirstFile((_path+"\\*").c_str(), &fileInformation);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				_fileNames.push_back(fileInformation.cFileName);
			} while(::FindNextFile(hFile, &fileInformation) == TRUE);
			::FindClose(hFile);
			std::vector<std::string> vFileNames = uListToVector(_fileNames);
			std::sort(vFileNames.begin(), vFileNames.end(), sortCallback);
			_fileNames = uVectorToList(vFileNames);
		}
#else
		int nameListSize;
		struct dirent ** nameList = 0;
		nameListSize =	scandir(_path.c_str(), &nameList, 0, sortCallback);
		if(nameList && nameListSize>0)
		{
			for (int i=0;i<nameListSize;++i)
			{
				_fileNames.push_back(nameList[i]->d_name);
				free(nameList[i]);
			}
			free(nameList);
		}
#endif

		//filter extensions...
		std::list<std::string>::iterator iter = _fileNames.begin();
		bool valid;
		while(iter!=_fileNames.end())
		{
			valid = false;
			if(_extensions.size() == 0 &&
			   iter->compare(".") != 0 &&
			   iter->compare("..") != 0)
			{
				valid = true;
			}
			for(unsigned int i=0; i<_extensions.size(); ++i)
			{
				if(UFile::getExtension(*iter).compare(_extensions[i]) == 0)
				{
					valid = true;
					break;
				}
			}
			if(!valid)
			{
				iter = _fileNames.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		_iFileName = _fileNames.begin();
	}
}

bool UDirectory::isValid()
{
	return exists(_path);
}

std::string UDirectory::getNextFileName()
{
	std::string fileName;
	if(_iFileName != _fileNames.end())
	{
		fileName = *_iFileName;
		++_iFileName;
	}
	return fileName;
}

void UDirectory::rewind()
{
	_iFileName = _fileNames.begin();
}


bool UDirectory::exists(const std::string & dirPath)
{
	bool r = false;
#if WIN32
	DWORD dwAttrib = GetFileAttributes(dirPath.c_str());
	r = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	DIR *dp;
	if((dp  = opendir(dirPath.c_str())) != NULL)
	{
		r = true;
		closedir(dp);
	}
#endif
	return r;
}

// return the directory path of the file
std::string UDirectory::getDir(const std::string & filePath)
{
	std::string dir = filePath;
	int i=dir.size()-1;
	for(; i>=0; --i)
	{
		if(dir[i] == '/' || dir[i] == '\\')
		{
			//remove separators...
			dir[i] = 0;
			--i;
			while(i>=0 && (dir[i] == '/' || dir[i] == '\\'))
			{
				dir[i] = 0;
				--i;
			}
			break;
		}
		else
		{
			dir[i] = 0;
		}
	}

	if(i<0)
	{
		dir = ".";
	}
	else
	{
		dir.resize(i+1);
	}

	return dir;
}

std::string UDirectory::currentDir(bool trailingSeparator)
{
	std::string dir;
	char * buffer;

#ifdef WIN32
	buffer = _getcwd(NULL, 0);
#else
	buffer = getcwd(NULL, MAXPATHLEN);
#endif

	if( buffer != NULL )
	{
		dir = buffer;
		free(buffer);
		if(trailingSeparator)
		{
			dir += "/";
		}
	}

	return dir;
}

bool UDirectory::makeDir(const std::string & dirPath)
{
	int status;
#if WIN32
	status = _mkdir(dirPath.c_str());
#else
	status = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	return status==0;
}

bool UDirectory::removeDir(const std::string & dirPath)
{
	int status;
#if WIN32
	status = _rmdir(dirPath.c_str());
#else
	status = rmdir(dirPath.c_str());
#endif
	return status==0;
}