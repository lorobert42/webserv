#include "AutoIndex.hpp"

AutoIndex::AutoIndex() {}

AutoIndex::AutoIndex(const std::string &uri, const std::string &pwd) :
	_uri(uri),
	_pwd(pwd)
{
	// Get all files and folders in path_folder
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(_pwd.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			// Get only files and folders that don't start with '.'
			if (ent->d_name[0] != '.')
			{
				// Push folder
				if (ent->d_type == DT_DIR)
					this->_folders.push_back(ent->d_name);
					// Push file
				else if (ent->d_type == DT_REG)
					this->_files.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	else
	{
		std::cout << "Could not open directory" << std::endl;
		return;
	}

	// Sort files and folders
	std::sort(this->_files.begin(), this->_files.end());
	std::sort(this->_folders.begin(), this->_folders.end());
}

AutoIndex::AutoIndex(const AutoIndex &src) {
	*this = src;
}

AutoIndex	&AutoIndex::operator=(const AutoIndex &rhs) {
	if (this != &rhs) {
		this->_uri = rhs._uri;
		this->_pwd = rhs._pwd;
		this->_body = rhs._body;
		this->_folders = rhs._folders;
		this->_files = rhs._files;
	}
	return *this;
}

AutoIndex::~AutoIndex() {}

std::string AutoIndex::getBody() {

	this->_body.append("<html>\n");
	this->_body.append("<head><title>");
	this->_body.append("Index of ");
	this->_body.append(this->_uri);
	this->_body.append("</title></head>\n");
	this->_body.append("<body>\n");
	this->_body.append("<h1>Index of ");
	this->_body.append(this->_uri);
	this->_body.append("</h1><hr><pre>");
	this->_body.append("<a href=\"../\">../</a>\n"); // Always present
	// Add folders to body
	for (std::vector<std::string>::iterator it = this->_folders.begin(); it != this->_folders.end(); ++it)
	{
		std::cout << "FOLDER: " << *it << std::endl;
		this->_body.append("<a href=\"");
		this->_body.append(*it);
		this->_body.append("/\">");
		this->_body.append(*it);
		this->_body.append("/</a>\n");
	}

	// Add files to body
	for (std::vector<std::string>::iterator it = this->_files.begin(); it != this->_files.end(); ++it)
	{
		std::cout << "FILE: " << *it << std::endl;
		this->_body.append("<a href=\"");
		this->_body.append(*it);
		this->_body.append("\">");
		this->_body.append(*it);
		this->_body.append("</a>\n");
	}


//	body.append("<a href=\"folder/\">folder/</a>                                           26-Oct-2023 09:39                   5\n");
//	body.append("<a href=\"file.txt\">file.txt</a>                                           26-Oct-2023 09:39                   5\n");
	this->_body.append("</pre><hr></body>\n");
	this->_body.append("</html>");
	return this->_body;
}