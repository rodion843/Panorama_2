#include <pylon/PylonIncludes.h>
#include <mutex>
#include <thread>
std::mutex img_mutex;

class PanoramaContinuousLineCameraConfig : public Pylon::CAcquireContinuousConfiguration{

};

void PylonThread()
{
    Pylon::PylonAutoInitTerm autoInitTerm;  // PylonInitialize() will be called now, destructor calls PylonTerminate()
    Pylon::CInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    auto nodemap = camera.GetNodeMap();

    Pylon::CIntegerParameter width(nodemap, "Width");
    Pylon::CIntegerParameter height(nodemap, "Height");
    Pylon::CIntegerParameter offsetX(nodemap, "OffsetX");
    Pylon::CIntegerParameter offsetY(nodemap, "OffsetY");
    width.TrySetValue(42);
    Pylon::CEnumParameter(nodemap, "PixelFormat").TrySetValue("Mono8");

    camera.RegisterConfiguration(new Pylon::CAcquireContinuousConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
    camera.StartGrabbing();
    Pylon::CGrabResultPtr ptrGrabResult;
    while (camera.IsGrabbing())
    {
        camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
        if(ptrGrabResult->GrabSucceeded())
        {
            img_mutex.lock();
            //opencv imshow();
            img_mutex.unlock();
        }
    }
}