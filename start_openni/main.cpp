#include <iostream>
#include<openni2/OpenNI.h>
#include"OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace std;

int main()
{
    openni::Status rc = openni::OpenNI::initialize();
    if(rc != openni::STATUS_OK)
    {
        cout<<"Initialize failed"<<endl<<openni::OpenNI::getExtendedError()<<endl;
        return 1;
    }
    openni::Device device;
    rc = device.open(openni::ANY_DEVICE);
    if(rc != openni::STATUS_OK)
    {
        cout<<"Couldn't open device"<<endl<<openni::OpenNI::getExtendedError()<<endl;
        return 2;
    }

    openni::VideoStream depth;

    if(device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device,openni::SENSOR_DEPTH);
        if(rc != openni::STATUS_OK)
        {
            cout<<"Couldn't create depth stream"<<endl<<openni::OpenNI::getExtendedError()<<endl;
            return 3;
        }
    }
    rc = depth.start();
    if(rc != openni::STATUS_OK)
    {
        cout<<"Couldn't start depth stream"<<endl<<openni::OpenNI::getExtendedError()<<endl;
        return 4;
    }

    openni::VideoFrameRef frame;

    while(!wasKeyboardHit())
    {
        int changedStreamDummy;
        openni::VideoStream* pStream = &depth;
        rc = openni::OpenNI::waitForAnyStream(&pStream,1,&changedStreamDummy,SAMPLE_READ_WAIT_TIMEOUT);
        if(rc != openni::STATUS_OK)
        {
            cout<<"Wait failed! (timeout is" <<SAMPLE_READ_WAIT_TIMEOUT<<"ms)"<<endl<<openni::OpenNI::getExtendedError()<<endl;
            continue;
        }

        rc = depth.readFrame(&frame);
        if (rc != openni::STATUS_OK)
        {
            cout<<"Read failed"<<endl<<openni::OpenNI::getExtendedError()<<endl;
            continue;
        }
        if(frame.getVideoMode().getPixelFormat() != openni::PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != openni::PIXEL_FORMAT_DEPTH_100_UM)
        {
            cout<<"Unexpected frame format"<<endl;
            continue;
        }

        openni::DepthPixel* pDepth = (openni::DepthPixel*)frame.getData();

        int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;
        cout<<"["<<(long long)frame.getTimestamp()<<"] "<< pDepth[middleIndex]<<endl;
    }
    depth.stop();
    depth.destroy();
    device.close();
    openni::OpenNI::shutdown();

    return 0;
}

