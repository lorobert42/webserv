#include "AutoIndex.hpp"

static std::string formatTime(time_t t) {
	struct tm* timeinfo;
	char buffer[80];
	timeinfo = localtime(&t);
	strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M", timeinfo);
	return std::string(buffer);
}

static std::string getFormattedDate(std::string resource_path) {
	struct stat fileStat;
	if (stat(resource_path.c_str(), &fileStat) == 0) {
		std::string formattedDate = formatTime(fileStat.st_mtime);
		return formattedDate;
	}
	return "";
}

static std::string getFileSize(const std::string &file_path) {
	struct stat fileStat;
	if (stat(file_path.c_str(), &fileStat) == 0) {
		std::stringstream ss;
		ss << fileStat.st_size;
		return ss.str();
	}
	return 0;
}

static void appendCorrectSpace(size_t contentLength, std::string &body, size_t nbOfChar) {
	// Append spaces to pad the content to 51 characters
	size_t spacesToAppend = nbOfChar - contentLength;
	body.append(std::string(spacesToAppend, ' '));
}

AutoIndex::AutoIndex() {}

AutoIndex::AutoIndex(const std::string &uri, const std::string &pwd) :
	_uri(uri),
	_pwd(pwd)
{
	std::cout << "uri: " << uri << std::endl;
	std::cout << "pwd: " << pwd << std::endl;
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
		this->_body.append("<a href=\"");
		this->_body.append(this->_uri);
		if (this->_uri[this->_uri.length() - 1] != '/')
			this->_body.append("/");
		this->_body.append(*it);
		this->_body.append("/\">");
		this->_body.append(*it);
		this->_body.append("/</a>");

		appendCorrectSpace(it->size() + 1, this->_body, 51);
		this->_body.append(getFormattedDate(this->_pwd + "/" + *it));

		this->_body.append("                   ");
		this->_body.append("-");

		this->_body.append("\n");
	}

	// Add files to body
	for (std::vector<std::string>::iterator it = this->_files.begin(); it != this->_files.end(); ++it)
	{
		this->_body.append("<a href=\"");
		this->_body.append(this->_uri);
		if (this->_uri[this->_uri.length() - 1] != '/')
			this->_body.append("/");
		this->_body.append(*it);
		this->_body.append("\">");
		this->_body.append(*it);
		this->_body.append("</a>");

		appendCorrectSpace(it->size(), this->_body, 51);
		this->_body.append(getFormattedDate(this->_pwd + "/" + *it));

		this->_body.append("                   ");
		this->_body.append(getFileSize(this->_pwd + "/" + *it));

		this->_body.append("\n");
	}

	this->_body.append("</pre><hr></body>\n");
	this->_body.append("</html>");
	return this->_body;
}