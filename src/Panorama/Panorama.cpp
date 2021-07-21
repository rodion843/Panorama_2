
#include <pylon/PylonIncludes.h>
#include <stdexcept>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <thread>
#include <mutex>
cv::Rect Box;
cv::Mat ResultImage;
cv::Mat frame;
std::mutex imshowMutex;
std::thread ImageGrabber;
void onMouse(int event, int x, int y, int, void *)
{
  switch (event) {
  case cv::MouseEventTypes::EVENT_LBUTTONDOWN:
    ImageGrabber = std::thread([&]() {
        imshowMutex.lock();
        std::cout << "thread 2\n";
        cv::Mat img = cv::imread("C:/Untitled.png");
        Box = cv::selectROI("roi sel", img);
        cv::Mat cropped = img(Box);
        cropped.copyTo(img(cv::Rect(0, 300, Box.width, Box.height)));
        imshowMutex.unlock();
    });
    break;
  }
}
int main(int /*argc*/, char * /*argv*/[])
{
  // The exit code of the sample application.


  Pylon::PylonInitialize();
  try {

    Pylon::PylonAutoInitTerm autoInitTerm;// PylonInitialize() will be called now, destructor calls PylonTerminate()
    Pylon::CInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;
    GenApi::INodeMap &nodemap = camera.GetNodeMap();
    camera.Open();
    Pylon::CIntegerParameter hearbeat(camera.GetTLNodeMap(), "HeartbeatTimeout");
    hearbeat.TrySetValue(2000, Pylon::IntegerValueCorrection_Nearest);
    camera.MaxNumBuffer = 100;
    Pylon::CGrabResultPtr ptrGrabResult;
    Pylon::CImageFormatConverter formatConverter;
    formatConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;
    Pylon::CPylonImage pylonImage;

    cv::namedWindow("soruce", cv::WindowFlags::WINDOW_AUTOSIZE);
    cv::setMouseCallback("soruce", onMouse, 0);
    //if (camera.GrabOne(5000, ptrGrabResult)) {
      //frame = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, pylonImage.GetBuffer());
    //}
    ResultImage = cv::Mat::zeros(cv::Size(/*frame.cols*/ 2500, /*frame.rows * 2*/ 600), CV_8UC1);

    camera.StartGrabbing();


    while (camera.IsGrabbing()) {
      camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
      if (ptrGrabResult->GrabSucceeded()) {
        formatConverter.Convert(pylonImage, ptrGrabResult);
        frame = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)pylonImage.GetBuffer());
        imshowMutex.lock();
        frame.copyTo(ResultImage(cv::Rect{ 0, 0, frame.cols, frame.rows }));
        cv::imshow("soruce", ResultImage);
        imshowMutex.unlock();
        cv::waitKey(1);
      } else {
        std::cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode() << std::dec << " " << ptrGrabResult->GetErrorDescription() << std::endl;
      }
    }


    ImageGrabber.join();
  } catch (const Pylon::GenericException &e) {
    // Error handling.
    std::cerr << "An exception occurred." << std::endl
              << e.GetDescription() << std::endl;
  } catch (std::exception e) {
    std::cout << "Generic exception: " << e.what() << "\n";
  }
  // Comment the following two lines to disable waiting on exit.
  // Releases all pylon resources.
  
  return 0;
}