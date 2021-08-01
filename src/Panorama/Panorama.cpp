
#include <pylon/PylonIncludes.h>
#include <stdexcept>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <string>

cv::Rect ROI_Selector;
cv::Mat ResultImage;
cv::Mat OneCameraFrame;

cv::Point Mouse_origin;
bool selectObject = false;
bool DrawRoiedImage;
cv::Rect selection;

const std::string WindowName = "source";

static bool showSelections() {
    cv::rectangle(ResultImage, selection, cv::Scalar(255, 255, 255), 5);
    cv::imshow(WindowName, ResultImage);
  return true;
}

void onMouse(int event, int x, int y, int, void *)
{

  switch (event) {
  case cv::MouseEventTypes::EVENT_LBUTTONDOWN:
    Mouse_origin = cv::Point(x, y);
    ResultImage = cv::Scalar(0, 0, 0);
    selectObject = true;
    DrawRoiedImage = false;
    showSelections();
    break;
  case cv::MouseEventTypes::EVENT_LBUTTONUP:
    DrawRoiedImage = true;
    selectObject = false;
    
    break;
  }
  if (selectObject) {
    selection.x = MIN(x, Mouse_origin.x);
    selection.y = MIN(y, Mouse_origin.y);
    selection.width = std::abs(x - Mouse_origin.x) +1;
    selection.height = std::abs(y - Mouse_origin.y) +1;
    selection &= cv::Rect(0, 0, OneCameraFrame.cols, OneCameraFrame.rows);
    if (selection.width > 0 && selection.height > 0) {
      ROI_Selector = selection;
    }
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

    cv::namedWindow(WindowName, cv::WindowFlags::WINDOW_NORMAL);
    cv::setMouseCallback(WindowName, onMouse, 0);
    //if (camera.GrabOne(5000, ptrGrabResult)) {
      //frame = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, pylonImage.GetBuffer());
    //}
    ResultImage = cv::Mat::zeros(cv::Size(/*frame.cols*/ 6000, /*frame.rows * 2*/ 1000), CV_8UC1);

    camera.StartGrabbing();


    while (camera.IsGrabbing()) {
      camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
      if (ptrGrabResult->GrabSucceeded()) {
        formatConverter.Convert(pylonImage, ptrGrabResult);
        OneCameraFrame = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)pylonImage.GetBuffer());
        OneCameraFrame.copyTo(ResultImage(cv::Rect{ 0, 0, OneCameraFrame.cols, OneCameraFrame.rows }));
        if (!selection.empty()) {
          cv::rectangle(ResultImage, selection, cv::Scalar(255, 255, 255), 5);
        }
        if (DrawRoiedImage) {
          cv::Mat roied_img = OneCameraFrame(cv::Rect{ 400, 40, 600, 80 });
          OneCameraFrame(ROI_Selector).copyTo(ResultImage(cv::Rect{ 0, 256 + 20, ROI_Selector.width, ROI_Selector.height}));
        }
        cv::imshow(WindowName, ResultImage);
        cv::waitKey(1);
      } else {
        std::cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode() << std::dec << " " << ptrGrabResult->GetErrorDescription() << std::endl;
      }
    }


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