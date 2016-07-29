#include "ccOpenVR.h"

#include <QMessageBox>

ccOpenVR::ccOpenVR()
{
    rendermodels=nullptr;
    fbo = new ccFrameBufferObject();
    session = false;
}

ccOpenVR::~ccOpenVR()
{
    delete fbo;
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
    if(!func.initializeOpenGLFunctions())
    {
        QMessageBox::critical(0,"Error","Cannot initialize OpenGL functions");
        return false;
    }
    return true;
}

void ccOpenVR::SetupCameras(float NearClip, float FarClip)
{
    mat4ProjectionLeft = HMD->GetProjectionMatrix(vr::Eye_Left,NearClip,FarClip,vr::API_OpenGL);
    mat4ProjectionRight = HMD->GetProjectionMatrix(vr::Eye_Right,NearClip,FarClip,vr::API_OpenGL);
    mat4eyePosLeft = HMD->GetEyeToHeadTransform(vr::Eye_Left);
    mat4eyePosRight =  HMD->GetEyeToHeadTransform(vr::Eye_Right);
}

bool ccOpenVR::SetupStereoRenderTargets()
{
    if(!HMD)
    {
        return false;
    }

    HMD->GetRecommendedRenderTargetSize(&RenderWidth,&RenderHeigh);

    /*if(!fbo->init(RenderWidth,RenderWidth))
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize framebuffer for OpenVR");
        return false;
    }
    if(!fbo->start())
    {
        QMessageBox::critical(0,"OpenVR", "Cannot bind framebuffer for OpenVR");
        return false;
    }*/

    if(!CreateFrameBuffer( RenderWidth, RenderWidth, leftEyeDesc ))
    {
        QMessageBox::critical(0,"Error","Cannot create framebuffer for left eye");
        return false;
    }
    if(!CreateFrameBuffer( RenderWidth, RenderWidth, rightEyeDesc ))
    {
        QMessageBox::critical(0,"Error","Cannot create framebuffer for right eye");
        return false;
    }

}

bool ccOpenVR::CreateFrameBuffer(uint32_t w, uint32_t h, FramebufferDesc buf)
{
    extfunc.glGenFramebuffers(1, &buf.m_nRenderFramebufferId );
    extfunc.glBindFramebuffer(GL_FRAMEBUFFER, buf.m_nRenderFramebufferId);

    extfunc.glGenRenderbuffers(1, &buf.m_nDepthBufferId);
    extfunc.glBindRenderbuffer(GL_RENDERBUFFER, buf.m_nDepthBufferId);
    extfunc.glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, w, h );
    extfunc.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	buf.m_nDepthBufferId );

    /*extfunc.glGenTextures(1, &buf.m_nRenderTextureId );
    extfunc.glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, buf.m_nRenderTextureId );
    extfunc.glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, w, h, true);
    extfunc.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, buf.m_nRenderTextureId, 0);*/

    extfunc.glGenFramebuffers(1, &buf.m_nResolveFramebufferId );
    extfunc.glBindFramebuffer(GL_FRAMEBUFFER, buf.m_nResolveFramebufferId);

    /*extfunc.glGenTextures(1, &buf.m_nResolveTextureId );
    extfunc.glBindTexture(GL_TEXTURE_2D, buf.m_nResolveTextureId );
    extfunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    extfunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    extfunc.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    extfunc.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buf.m_nResolveTextureId, 0);*/

    // check FBO status
    GLenum status = extfunc.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        return false;
    }

    extfunc.glBindFramebuffer( GL_FRAMEBUFFER, 0 );

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
