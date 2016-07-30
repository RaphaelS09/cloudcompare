#include "ccOpenVR.h"

#include <QMessageBox>

ccOpenVR::ccOpenVR()
{
    rendermodels=nullptr;
    session = false;
    fboLeft = new ccFrameBufferObject;
    fboRight = new ccFrameBufferObject;
}

ccOpenVR::~ccOpenVR()
{
    delete fboLeft;
    delete fboRight;
}

bool ccOpenVR::init()
{
    vr::EVRInitError error=vr::VRInitError_None;
    HMD = vr::VR_Init(&error, vr::VRApplication_Scene);
    if(error != vr::VRInitError_None)
    {
        HMD=nullptr;
        QMessageBox::critical(0,"VR_Init Failed",QString("Unable to init VR runtime: %1").arg(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
        return false;
    }
    rendermodels = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
    if(!rendermodels)
    {
        HMD=nullptr;
        vr::VR_Shutdown();
        QMessageBox::critical(0,"VR_Init Failed",QString("Unable to get render model interface: %1").arg(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
        return false;
    }
    HMD->GetRecommendedRenderTargetSize(&RenderWidth,&RenderHeigh);

    return true;
}

void ccOpenVR::SetupCameras(float NearClip, float FarClip)
{
    mat4ProjectionLeft = HMD->GetProjectionMatrix(vr::Eye_Left,NearClip,FarClip,vr::API_OpenGL);
    mat4ProjectionRight = HMD->GetProjectionMatrix(vr::Eye_Right,NearClip,FarClip,vr::API_OpenGL);
    mat4eyePosLeft = HMD->GetEyeToHeadTransform(vr::Eye_Left);
    mat4eyePosRight =  HMD->GetEyeToHeadTransform(vr::Eye_Right);
}

bool ccOpenVR::SetupStereoRenderTargets(QOpenGLContext *context)
{
    if(!HMD)
    {
        return false;
    }

    if(!SetupRenderTarget(fboLeft,context))
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize left framebuffer for OpenVR");
        return false;
    }

    if(!SetupRenderTarget(fboRight,context))
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize right framebuffer for OpenVR");
        return false;
    }

    return true;
}

bool ccOpenVR::SetupRenderTarget(ccFrameBufferObject *fbo, QOpenGLContext *context)
{
    QOpenGLFunctions_2_1 *func = context->versionFunctions<QOpenGLFunctions_2_1>();
    unsigned int texId;
    func->glGenTextures(1, &texId);
    func->glBindTexture(GL_TEXTURE_2D, texId);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR/*GL_LINEAR_MIPMAP_LINEAR*/);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (context->hasExtension(QByteArrayLiteral("GLE_EXT_texture_filter_anisotropic")))
    {
        func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
    }
    func->glBindTexture(GL_TEXTURE_2D, 0);

    func->glPushAttrib(GL_ENABLE_BIT);
    func->glEnable(GL_TEXTURE_2D);

    GLuint texID = 0;
    func->glGenTextures(1, &texID);
    func->glBindTexture(GL_TEXTURE_2D, texID);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    func->glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    func->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    func->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, RenderWidth, RenderHeigh, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    func->glBindTexture(GL_TEXTURE_2D, 0);

    func->glPopAttrib();


    /*if(!fbo->initColor())
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize framebuffer color for OpenVR");
        return false;
    }
    if(!fbo->initDepth())
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize framebuffer depth for OpenVR");
        return false;
    }*/
    return true;
}

ccGLMatrixd HmdMat44toccGLMatd(vr::HmdMatrix44_t m)
{
    ccGLMatrixd mat;
    for(int i=0;i<4;++i)
    {
        mat.setColumn(i,Tuple4Tpl<double>(m.m[i][0],m.m[i][1],m.m[i][2],m.m[i][3]));
    }
    return mat;
}

ccGLMatrixd HmdMat34toccGLMatd(vr::HmdMatrix34_t m)
{
    ccGLMatrixd mat;
    for(int i=0;i<3;++i)
    {
        mat.setColumn(i,Tuple4Tpl<double>(m.m[i][0],m.m[i][1],m.m[i][2],m.m[i][3]));
    }
    mat.setColumn(3,Tuple4Tpl<double>(0.0f,0.0f,0.0f,1.0f));
    return mat;
}
