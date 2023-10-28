#include "CLI.hpp"


CLI::CLI()
:ipAddress(), port()
{

}


bool CLI::autoStart(cv::VideoCapture& camera)
{
    std::string url = "http://192.168.1.6:4747/mjpegfeed?960x720";
    camera.open(url);
    return camera.isOpened();
}

bool CLI::startCLI(cv::VideoCapture& camera)
{
    std::cout << "OPAL OMR Version 0.0.2" << '\n';
    std::cout << "Enter IP address and port number" << '\n';


    bool connectionEntry = true;
    bool connected = true;
    while(connectionEntry)
    {    
        std::cout << "IPAddress : "; std::getline(std::cin, ipAddress);
        std::cout << "Port : ";  std::getline(std::cin, port);

        std::string url = ("http://") + ipAddress + (":") + port + ("/mjpegfeed?960x720");
        
        camera.open(url);
        if(camera.isOpened() == false)
        {  
            std::cout << "Error while connecting to the camera" << '\n';
			std::cout << "Make sure that you have wrote the IP address and the port number correctly." << '\n';
			std::cout << "Leave?" << "(y/n)" << '\n';
			std::string outputString;
			std::cin >> outputString;

			// if the length isn't greater by one and if the value of the output string is either captalized or small. describes the 
			// below if statement
			if((outputString.length() > 1) == false && ( (outputString == "y") == true || (outputString == "Y") ) == true )
            {
                connected = false;
                connectionEntry = false;
            }
			if( (outputString.length() > 1) == false && ((outputString == "n") == true || (outputString == "N") == true ) == true)
				connectionEntry = true;
        }

        if(camera.isOpened() == true)
        {
            std::cout << "Connected" << '\n';
            connectionEntry = false;
        }
    }
    return connected;    
}

