#include "CLI.hpp"


CLI::CLI()
{

}


bool CLI::autoStart(cv::VideoCapture& camera)
{
    std::string url = "http://192.168.1.6:4747/video?960x720";
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
        std::cout << "IPAddress : "; std::getline(std::cin, _ipAddress);
        std::cout << "Port : ";  std::getline(std::cin, _port);
        std::string url = ("http://") + _ipAddress + (":") + _port + ("/video?960x720");
        
        camera.open(url);
        if(camera.isOpened() == false)
        {  
            std::cout << "Error while connecting to the camera" << '\n';
			std::cout << "Make sure that you have wrote the IP address and the port number correctly." << '\n';
			std::cout << "Leave?" << "(y/n)" << '\n';
			std::string outputString;
			std::getline(std::cin, outputString);
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


// OPTIONS CLI::startCLI(cv::VideoCapture& camera)
// {
//     std::cout << "OPAL - Version 0.0.2" << '\n';
//     OPTIONS currentOptions;
//     while(1)
//     {
//         std::cout << "1. Video Processing" << '\n';
//         std::cout << "2. Preloaded Image" << '\n';

//         std::string output = "";
//         char outputValue = '_';
//         std::getline(std::cin, output);

//         if(output.length() != 1)
//             std::cout << "Error while parsing ' " << output << " '" << '\n'; 
//         else if(output.length() == 1)
//         {
//             int value = static_cast<int>(output[0]);
//             // 48 and 57 are ASCII values for all numerals
//             if(value >= 48 && value <= 57)
//                 outputValue = static_cast<char>(value);

//             if((value >= 48 && value <= 57) == false)
//                 std::cout << "Error while parsing ' " << output << " ', make sure it is a numeral" << '\n'; 
//         }
//             std::cout << static_cast<int>(output[0]) << "  ";

//         std::cout << outputValue << '\n'; 

//         // checking the output

//         if(outputValue == '1')
//         {
//             // start the connection protocol
//             std::cout << "IPAddress : "; std::getline(std::cin, _ipAddress);
//             std::cout << "Port : "; std::getline(std::cin, _port);

//             std::string url = ("http://") + _ipAddress + (":") + _port + ("/video?960x720");
//             camera.open(url);
//             if(camera.isOpened() == true)
//             {
//                 std::cout << "Connected :D" << '\n';
//                 return VIDEO;
//             }
//             // continues and resets to the old path.
//             if(camera.isOpened() == false)
//                 outputValue = -1;                   
//         }
//         else if(outputValue == '2')
//         {
//             // prompt the user if it wants a single processing option or a multiple one
//             std::cout << "Do you want to process a single image or a cluster?" << '\n';
//             std::cout << "([S]ingle Image / [C]luster images)" << '\n';
//             bool childLoop = true;
//             while(childLoop)
//             {
//                 std::string output = "";
//                 std::getline(std::cin, output);
                                
//                 if(output.length() != 0)
//                     std::cout << "Error while parsing ' " << output << " ', make sure it is either C or S" << '\n'; 
                
//                 else if(output.length() == 0)
//                 {
//                     if(output[0] == 'S' || output[0] == 's')
//                     {
//                         currentOptions = IMAGE;
//                         break;
//                     }

//                     if(output[0] == 'C' || output[0] == 'c')
//                     {
//                         currentOptions = IMAGE_LIST;
//                         break;
//                     }
//                     else{
//                         std::cout << "Error while parsing ' " << output << " ', make sure it is either C or S" << '\n'; 
//                     }
//                 }
//             }

//             break;
//         }
//         else if(outputValue != '_' || outputValue != '1' || outputValue != '2')
//         {
//             // tell the user how stupid it is for choosing the wrong option.
//         }

        
//     }
// }

