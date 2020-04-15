#include <util.hh>

string util::strip(const string &str)
{
	auto b(str.begin());
	auto e(str.end());
	if(b==e)
		return str;
	while(b!=e && isspace(*--e))
		;
	while(b!=e && isspace(*b))
		++b;
	return string(b,++e);
};
string util::read_file(const char *name)
{
	ifstream file(name);
	std::stringstream buf;
	buf << file.rdbuf();
	string res=buf.str();
	cout << "read " << res.length() << " bytes" << endl;
	return res;
};
bool util::exists(const char *name)
{
	struct stat buf;
	if(stat(name,&buf)<0){
		return false;
	};
	return true;
};

