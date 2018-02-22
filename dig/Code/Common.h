
#include <fstream>
#include <string>

class CCommon
{
public:
	std::ofstream of;

	void InitHtml()
	{
		
		of.open("data.html");
		of << "<html><body style='font-family:Arial;margin:0px;background-color:#161921;color:#ced4e5;'><div style='background-color:#242834;height:53px;box-shadow:0px 0px 7px #000;'><br>&nbsp;&nbsp;&nbsp;<label style='color:white;margin-top:-5px;font-size:19px;'>Data</label></div><br>";
	}

	void InsertHtml(std::string str)
	{
		of << "&nbsp;&nbsp;&nbsp;" << str << "<br><hr style='border-width:0.5px;border-color:#2a303f;'>";
	}

	void EndHtml()
	{
		of << "</html></body>";

		of.close();
	}
};
