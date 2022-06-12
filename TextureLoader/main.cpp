#include "stb_image.h"
#include <iostream>
#include <fstream>

int main() 
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char *data = stbi_load("../res/textures/faerun_no_tags.jpg", &width, &height, &nrChannels, 0);
	
	std::ofstream myfile;
	myfile.open("../Cartographer/test.h");
	myfile << "struct texture {\n";
	myfile << "\tint height = " << height << ";\n";
	myfile << "\tint width = " << width << ";\n";
	myfile << "\tchar data[" << height*width << "] = \"\n"; 
	for (int x = 0; x < height*width; ++x) 
	{
		myfile << data[x];
	}
	myfile << "\";} tex;\n";
	myfile.close();
	
	return 0;
}