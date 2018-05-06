#include "knt_viewer.h"
#include "iostream"

int main(int argc, char const *argv[])
{
	try {
		if(argc != 2) {
			std::cout << "Usage: ./kntViewer <kntFilePath>" << std::endl;
			return 0;
		}

		KntViewer knt_viewer;
		std::cout << "File path is: " << argv[1] << std::endl;
		knt_viewer.init(argv[1]);
		std::cout << "init complete." << std::endl;
		knt_viewer.run();
		// std::cout << "Hello world!" << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}