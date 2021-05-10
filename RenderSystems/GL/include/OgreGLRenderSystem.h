/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __GLRenderSystem_H__
#define __GLRenderSystem_H__

#include "OgreGLPrerequisites.h"
#include "OgrePlatform.h"
#include "OgreRenderSystem.h"
#include "OgreGLHardwareBufferManager.h"
#include "OgreGLGpuProgramManager.h"
#include "OgreVector.h"

#include "OgreGLRenderSystemCommon.h"
#include "OgreGLStateCacheManager.h"
#include "OgreCamera.h"
#include "OgreSceneManager.h"
#include "OgreRenderTargetListener.h"

namespace Ogre {
    /** \addtogroup RenderSystems RenderSystems
    *  @{
    */
    /** \defgroup GL GL
    * Implementation of GL as a rendering system.
    *  @{
    */

    namespace GLSL {
        class GLSLProgramFactory;
    }

    /**
      Implementation of GL as a rendering system.
     */
    class _OgreGLExport GLRenderSystem : public GLRenderSystemCommon,
                                         public Camera::Listener,
                                         public RenderTargetListener,
                                         public SceneManager::Listener
    {
    private:

        /// View matrix to set world against
        Matrix4 mViewMatrix;
        Matrix4 mWorldMatrix;
        Matrix4 mTextureMatrix;

        /// Last min & mip filtering options, so we can combine them
        FilterOptions mMinFilter;
        FilterOptions mMipFilter;

        /// What texture coord set each texture unit is using
        size_t mTextureCoordIndex[OGRE_MAX_TEXTURE_LAYERS];

        /// Holds texture type settings for every stage
        GLenum mTextureTypes[OGRE_MAX_TEXTURE_LAYERS];

        /// Number of fixed-function texture units
        unsigned short mFixedFunctionTextureUnits;

        void setGLLight(size_t index, bool lt);
        void makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m);
 
        GLint getBlendMode(SceneBlendFactor ogreBlend) const;
        GLint getTextureAddressingMode(TextureAddressingMode tam) const;
                void initialiseContext(RenderWindow* primary);

        /// Store last stencil mask state
        uint32 mStencilWriteMask;
        /// Store last depth write state
        bool mDepthWrite;
        /// Rendering loop control
        bool mStopRendering;

        /// Render frames using quad buffer instead of standard double buffer.
        bool mQuadBufferEnabled;

        /// Should we draw on left or right back buffer when quad buffer is activated?
        bool mIsCurrentBufferRight;

        /// As we activate the quad buffer on a camera after it's pre-render event,
        /// we need a flag to indicate that we will ignore the first frame rendering.
        bool mIsQuadBufferInitialized;

        /// Flag to know when we are rendering textures shadows with quad buffer enabled.
        bool mIsQuadBufferShadowsRendering;

        /// Store a list of cameras, usefull for quad buffer camera listeners add/deletion.
        typedef std::set<Camera*> CameraList;
        CameraList mRegisteredQuadBufferCameras;

        typedef std::set<RenderTarget*> ViewportList;
        ViewportList mRegisteredQuadBufferViewport;

        /// Store a list of scene manager, usefull for quad buffer scene manager listeners add/deletion.
        typedef std::set<SceneManager*> SceneManagerList;
        SceneManagerList mRegisteredQuadBufferSceneManagers;


        GLint convertCompareFunction(CompareFunction func) const;
        GLint convertStencilOp(StencilOperation op, bool invert = false) const;

        bool mUseAutoTextureMatrix;
        GLfloat mAutoTextureMatrix[16];

        /// Check if the GL system has already been initialised
        bool mGLInitialised;

        HardwareBufferManager* mHardwareBufferManager;
        GLGpuProgramManager* mGpuProgramManager;
        GLSL::GLSLProgramFactory* mGLSLProgramFactory;

        unsigned short mCurrentLights;

        GLGpuProgramBase* mCurrentVertexProgram;
        GLGpuProgramBase* mCurrentFragmentProgram;
        GLGpuProgramBase* mCurrentGeometryProgram;

        // statecaches are per context
        GLStateCacheManager* mStateCacheManager;

        ushort mActiveTextureUnit;
        ushort mMaxBuiltInTextureAttribIndex;

        // local data members of _render that were moved here to improve performance
        // (save allocations)
        std::vector<GLuint> mRenderAttribsBound;
        std::vector<GLuint> mRenderInstanceAttribsBound;

        /// is fixed pipeline enabled
        bool mEnableFixedPipeline;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		/// @copydoc RenderSystem::setDrawBuffer
		virtual bool setDrawBuffer(ColourBufferType colourBuffer);
#endif

    protected:
        void setClipPlanesImpl(const PlaneList& clipPlanes);
        void bindVertexElementToGpu(const VertexElement& elem,
                                    const HardwareVertexBufferSharedPtr& vertexBuffer,
                                    const size_t vertexStart);

        /** Initialises GL extensions, must be done AFTER the GL context has been
            established.
        */
        void initialiseExtensions();
    public:
        // Default constructor / destructor
        GLRenderSystem();
        ~GLRenderSystem();

        // ----------------------------------
        // Overridden RenderSystem functions
        // ----------------------------------

        const GpuProgramParametersPtr& getFixedFunctionParams(TrackVertexColourType tracking, FogMode fog);

        void applyFixedFunctionParams(const GpuProgramParametersPtr& params, uint16 variabilityMask);

        const String& getName(void) const;

        void _initialise() override;

        void initConfigOptions() override;

        virtual RenderSystemCapabilities* createRenderSystemCapabilities() const;

        void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary);

        void shutdown(void);

        void setShadingType(ShadeOptions so);

        void setLightingEnabled(bool enabled);
        
        /// @copydoc RenderSystem::_createRenderWindow
        RenderWindow* _createRenderWindow(const String &name, unsigned int width, unsigned int height, 
                                          bool fullScreen, const NameValuePairList *miscParams = 0);

        /// @copydoc RenderSystem::_createDepthBufferFor
        DepthBuffer* _createDepthBufferFor( RenderTarget *renderTarget );
        
        /// @copydoc RenderSystem::createMultiRenderTarget
        virtual MultiRenderTarget * createMultiRenderTarget(const String & name); 
        

        void destroyRenderWindow(const String& name);

        void setNormaliseNormals(bool normalise);

        // -----------------------------
        // Low-level overridden members
        // -----------------------------

        void _useLights(unsigned short limit);

        void setWorldMatrix(const Matrix4 &m);

        void setViewMatrix(const Matrix4 &m);

        void setProjectionMatrix(const Matrix4 &m);

        void _setSurfaceTracking(TrackVertexColourType tracking);

        void _setPointParameters(bool attenuationEnabled, Real minSize, Real maxSize);

        void _setLineWidth(float width);

        void _setPointSpritesEnabled(bool enabled);

        void _setTexture(size_t unit, bool enabled, const TexturePtr &tex);

        void _setSampler(size_t unit, Sampler& sampler);

        void _setTextureCoordSet(size_t stage, size_t index);

        void _setTextureCoordCalculation(size_t stage, TexCoordCalcMethod m, 
            const Frustum* frustum = 0);

        void _setTextureBlendMode(size_t stage, const LayerBlendModeEx& bm);

        void _setTextureAddressingMode(size_t stage, const Sampler::UVWAddressingMode& uvw);

        void _setTextureMatrix(size_t stage, const Matrix4& xform);

        void _setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage);

        void _setViewport(Viewport *vp);

        void _endFrame(void);

        void _setCullingMode(CullingMode mode);

        void _setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);

        void _setDepthBufferCheckEnabled(bool enabled = true);

        void _setDepthBufferWriteEnabled(bool enabled = true);

        void _setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);

        void _setDepthBias(float constantBias, float slopeScaleBias);

        void setColourBlendState(const ColourBlendState& state);

        void _setFog(FogMode mode);

        void setClipPlane (ushort index, Real A, Real B, Real C, Real D);

        void enableClipPlane (ushort index, bool enable);

        void _setPolygonMode(PolygonMode level);

        void setStencilCheckEnabled(bool enabled);
        /** See
          RenderSystem.
         */
        void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, 
            uint32 refValue = 0, uint32 compareMask = 0xFFFFFFFF, uint32 writeMask = 0xFFFFFFFF,
            StencilOperation stencilFailOp = SOP_KEEP, 
            StencilOperation depthFailOp = SOP_KEEP,
            StencilOperation passOp = SOP_KEEP, 
            bool twoSidedOperation = false,
            bool readBackAsTexture = false);

        void _setTextureUnitFiltering(size_t unit, FilterType ftype, FilterOptions filter);

        void _render(const RenderOperation& op);

        void bindGpuProgram(GpuProgram* prg);

        void unbindGpuProgram(GpuProgramType gptype);

        void bindGpuProgramParameters(GpuProgramType gptype, 
                                      const GpuProgramParametersPtr& params, uint16 variabilityMask);

        void setScissorTest(bool enabled, const Rect& rect = Rect()) ;
        void clearFrameBuffer(unsigned int buffers, 
                              const ColourValue& colour = ColourValue::Black, 
                              Real depth = 1.0f, unsigned short stencil = 0);
        HardwareOcclusionQuery* createHardwareOcclusionQuery(void);

        // ----------------------------------
        // GLRenderSystem specific members
        // ----------------------------------
        void _oneTimeContextInitialization();
        /** Switch GL context, dealing with involved internal cached states too
        */
        void _switchContext(GLContext *context);
        /**
         * Set current render target to target, enabling its GL context if needed
         */
        void _setRenderTarget(RenderTarget *target);
        /** Unregister a render target->context mapping. If the context of target 
            is the current context, change the context to the main context so it
            can be destroyed safely. 
            
            @note This is automatically called by the destructor of 
            GLContext.
         */
        void _unregisterContext(GLContext *context);

        GLStateCacheManager * _getStateCacheManager() { return mStateCacheManager; }
        
        /// @copydoc RenderSystem::beginProfileEvent
        virtual void beginProfileEvent( const String &eventName );

        /// @copydoc RenderSystem::endProfileEvent
        virtual void endProfileEvent( void );

        /// @copydoc RenderSystem::markProfileEvent
        virtual void markProfileEvent( const String &eventName );

        /** See Camera::cameraPreRenderScene
        @note This callback is only usefull if you have activated the quad buffer rendering option.		
        */		
        virtual void cameraPreRenderScene(Camera* cam);		

        /** @copydoc RenderTarget::copyContentsToMemory */
        void _copyContentsToMemory(Viewport* vp, const Box& src, const PixelBox &dst, RenderWindow::FrameBuffer buffer);
        
        /** See RenderTargetListener::preViewportUpdate
        @note This callback is only usefull if you have activated the quad buffer rendering option.		
        */		
        virtual void preRenderTargetUpdate(const RenderTargetEvent& evt);	

        /** See RenderTargetListener::preViewportUpdate		
        @note This callback is only usefull if you have activated the quad buffer rendering option.		
        */		
        virtual void postRenderTargetUpdate(const RenderTargetEvent& evt);	

        /** See RenderTargetListener::viewportRemoved		
        @note This callback is only usefull if you have activated the quad buffer rendering option.		
        */		
        virtual void viewportRemoved(const RenderTargetViewportEvent& evt);

        /** See Camera::cameraDestroyed
        @note This callback is only usefull if you have activated the quad buffer rendering option.
        */
        virtual void cameraDestroyed(Camera* cam);

        /** See SceneManager::shadowTextureCasterPreViewProj
        @note This callback is only usefull if you have activated the quad buffer rendering option, and we render a shadow texture
        */
        virtual void shadowTextureCasterPreViewProj(Light* light, Camera* camera, size_t iteration);

        /** See SceneManager::shadowTextureReceiverPreViewProj
        @note This callback is only usefull if you have activated the quad buffer rendering option, and we render a shadow texture
        */
        virtual void shadowTexturesUpdated(size_t numberOfShadowTextures);

        /** See SceneManager::sceneManagerDestroyed
        @note This callback is only usefull if you have activated the quad buffer rendering option, and we render a shadow texture
        */
        virtual void sceneManagerDestroyed(SceneManager* source);

    };
    /** @} */
    /** @} */
}
#endif

