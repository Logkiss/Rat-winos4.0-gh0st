//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx9anim.h
//  Content:    D3DX mesh types and functions
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __D3DX9ANIM_H__
#define __D3DX9ANIM_H__

// {ADE2C06D-3747-4b9f-A514-3440B8284980}
DEFINE_GUID(IID_ID3DXInterpolator, 
0xade2c06d, 0x3747, 0x4b9f, 0xa5, 0x14, 0x34, 0x40, 0xb8, 0x28, 0x49, 0x80);

// {6CAA71F8-0972-4cdb-A55B-43B968997515}
DEFINE_GUID(IID_ID3DXKeyFrameInterpolator, 
0x6caa71f8, 0x972, 0x4cdb, 0xa5, 0x5b, 0x43, 0xb9, 0x68, 0x99, 0x75, 0x15);

// {54B569AC-0AEF-473e-9704-3FEF317F64AB}
DEFINE_GUID(IID_ID3DXAnimationSet, 
0x54b569ac, 0xaef, 0x473e, 0x97, 0x4, 0x3f, 0xef, 0x31, 0x7f, 0x64, 0xab);

// {3A714D34-FF61-421e-909F-639F38356708}
DEFINE_GUID(IID_ID3DXAnimationController, 
0x3a714d34, 0xff61, 0x421e, 0x90, 0x9f, 0x63, 0x9f, 0x38, 0x35, 0x67, 0x8);


typedef struct ID3DXInterpolator *LPD3DXINTERPOLATOR;
typedef struct ID3DXKeyFrameInterpolator *LPD3DXKEYFRAMEINTERPOLATOR;
typedef struct ID3DXAnimationSet *LPD3DXANIMATIONSET;
typedef struct ID3DXAnimationController *LPD3DXANIMATIONCONTROLLER;

typedef struct ID3DXAllocateHierarchy *LPD3DXALLOCATEHIERARCHY;
typedef struct ID3DXLoadUserData *LPD3DXLOADUSERDATA;
typedef struct ID3DXSaveUserData *LPD3DXSAVEUSERDATA;

//----------------------------------------------------------------------------
// This enum defines the type of mesh data present in a MeshData structure
//----------------------------------------------------------------------------
typedef enum _D3DXMESHDATATYPE {
    D3DXMESHTYPE_MESH      = 0x001,     // normal ID3DXMesh data
    D3DXMESHTYPE_PMESH     = 0x002,     // Progressive Mesh - ID3DXPMesh
    D3DXMESHTYPE_PATCHMESH = 0x003,     // Patch MEsh - ID3DXPatchMesh

    D3DXMESHTYPE_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DXMESHDATATYPE;

//----------------------------------------------------------------------------
// This struct encapsulates a the mesh data that can be present in a mesh 
//   container.  The supported mesh types are pMesh, pPMesh, pPatchMesh
//   The valid way to access this is determined by the MeshType enum
//----------------------------------------------------------------------------
typedef struct _D3DXMESHDATA
{
    D3DXMESHDATATYPE Type;

    // current mesh data interface
    union
    {
        LPD3DXMESH              pMesh;
        LPD3DXPMESH             pPMesh;
        LPD3DXPATCHMESH         pPatchMesh;
    };

} D3DXMESHDATA, *LPD3DXMESHDATA;

//----------------------------------------------------------------------------
// This struct encapsulates a mesh object in a transformation frame
// hierarchy. The app can derive from this structure to add other app specific
// data to this
//----------------------------------------------------------------------------
typedef struct _D3DXMESHCONTAINER
{
    LPSTR                   Name;

    D3DXMESHDATA            MeshData;

    LPD3DXMATERIAL          pMaterials;
    LPD3DXEFFECTINSTANCE    pEffects;
    DWORD                   NumMaterials;
    DWORD                  *pAdjacency;

    LPD3DXSKININFO          pSkinInfo;

    struct _D3DXMESHCONTAINER *pNextMeshContainer;
} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;

//----------------------------------------------------------------------------
// This struct is the encapsulates a transform frame in a transformation frame
// hierarchy. The app can derive from this structure to add other app specific
// data to this
//----------------------------------------------------------------------------
typedef struct _D3DXFRAME
{
    LPSTR                   Name;
    D3DXMATRIX              TransformationMatrix;

    LPD3DXMESHCONTAINER     pMeshContainer;

    struct _D3DXFRAME       *pFrameSibling;
    struct _D3DXFRAME       *pFrameFirstChild;
} D3DXFRAME, *LPD3DXFRAME;

#undef INTERFACE
#define INTERFACE ID3DXAllocateHierarchy

//----------------------------------------------------------------------------
// This interface is implemented by the application to allocate/free frame and
// mesh container objects. Methods on this are called during loading and
// destroying frame hierarchies
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXAllocateHierarchy ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE(ID3DXAllocateHierarchy)
{
    // ID3DXAllocateHierarchy

	//------------------------------------------------------------------------
	// CreateFrame:
	// ------------
	// Requests allocation of a frame object.
	//
	// Parameters:
	//  Name
	//		Name of the frame to be created
	//	ppNewFrame
	//		Returns returns the created frame object
	//
	//------------------------------------------------------------------------
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, 
                            LPD3DXFRAME *ppNewFrame) PURE;

	//------------------------------------------------------------------------
	// CreateMeshContainer:
	// --------------------
	// Requests allocation of a mesh container object.
	//
	// Parameters:
	//  Name
	//		Name of the mesh
	//	pMesh
	//		Pointer to the mesh object if basic polygon data found
	//	pPMesh
	//		Pointer to the progressive mesh object if progressive mesh data found
	//	pPatchMesh
	//		Pointer to the patch mesh object if patch data found
	//	pMaterials
	//		Array of materials used in the mesh
	//	pEffectInstances
	//		Array of effect instances used in the mesh
	//	NumMaterials
	//		Num elements in the pMaterials array
	//	pAdjacency
	//		Adjacency array for the mesh
	//	pSkinInfo
	//		Pointer to the skininfo object if the mesh is skinned
	//	pBoneNames
	//		Array of names, one for each bone in the skinned mesh. 
	//		The numberof bones can be found from the pSkinMesh object
	//	pBoneOffsetMatrices
	//		Array of matrices, one for each bone in the skinned mesh.
	//
	//------------------------------------------------------------------------
    STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, LPD3DXMESHDATA pMeshData, 
                            LPD3DXMATERIAL pMaterials, LPD3DXEFFECTINSTANCE pEffectInstances, DWORD NumMaterials, 
                            DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, 
                            LPD3DXMESHCONTAINER *ppNewMeshContainer) PURE;

	//------------------------------------------------------------------------
	// DestroyFrame:
	// -------------
	// Requests de-allocation of a frame object.
	//
	// Parameters:
	//  pFrameToFree
	//		Pointer to the frame to be de-allocated
	//
	//------------------------------------------------------------------------
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) PURE; 

	//------------------------------------------------------------------------
	// DestroyMeshContainer:
	// ---------------------
	// Requests de-allocation of a mesh container object.
	//
	// Parameters:
	//  pMeshContainerToFree
	//		Pointer to the mesh container object to be de-allocated
	//
	//------------------------------------------------------------------------
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree) PURE; 
};

//----------------------------------------------------------------------------
// This interface is implemented by the application to load user data in a .X file
//   When user data is found, these callbacks will be used to allow the application
//   to load the data
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXLoadUserData ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE(ID3DXLoadUserData)
{
    STDMETHOD(LoadTopLevelData)(LPDIRECTXFILEDATA pXofChildData) PURE;
                            
    STDMETHOD(LoadFrameChildData)(LPD3DXFRAME pFrame, 
                            LPDIRECTXFILEDATA pXofChildData) PURE;
                            
    STDMETHOD(LoadMeshChildData)(LPD3DXMESHCONTAINER pMeshContainer, 
                            LPDIRECTXFILEDATA pXofChildData) PURE;                            
};

//----------------------------------------------------------------------------
// This interface is implemented by the application to save user data in a .X file
//   The callbacks are called for all data saved.  The user can then add any
//   child data objects to the object provided to the callback
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXSaveUserData /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE(ID3DXSaveUserData)
{
    STDMETHOD(AddFrameChildData)(LPD3DXFRAME pFrame, 
                            LPDIRECTXFILESAVEOBJECT pXofSave, 
                            LPDIRECTXFILEDATA pXofFrameData) PURE;
                            
    STDMETHOD(AddMeshChildData)(LPD3DXMESHCONTAINER pMeshContainer, 
                            LPDIRECTXFILESAVEOBJECT pXofSave, 
                            LPDIRECTXFILEDATA pXofMeshData) PURE;
                            
    // NOTE: this is called once per Save.  All top level objects should be added using the 
    //    provided interface.  One call adds objects before the frame hierarchy, the other after
    STDMETHOD(AddTopLevelDataObjectsPre)(LPDIRECTXFILESAVEOBJECT pXofSave) PURE; 
    STDMETHOD(AddTopLevelDataObjectsPost)(LPDIRECTXFILESAVEOBJECT pXofSave) PURE;                             

    // callbacks for the user to register and then save templates to the XFile
    STDMETHOD(RegisterTemplates)(LPDIRECTXFILE pXFileApi) PURE;                             
    STDMETHOD(SaveTemplates)(LPDIRECTXFILESAVEOBJECT pXofSave) PURE;                             
};

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


//----------------------------------------------------------------------------
// D3DXLoadMeshHierarchyFromX:
// ---------------------------
// Loads the first frame hierarchy in a .X file.
//
// Parameters:
//  Filename
//      Name of the .X file
//  MeshOptions
//      Mesh creation options for meshes in the file (see d3dx9mesh.h)
//  pD3DDevice
//      D3D9 device on which meshes in the file are created in
//  pAlloc
//      Allocation interface used to allocate nodes of the frame hierarchy
//  pUserDataLoader
//      Application provided interface to allow loading of user data
//  ppFrameHierarchy
//      Returns root node pointer of the loaded frame hierarchy
//  ppAnimController
//      Returns pointer to an animation controller corresponding to animation
//		in the .X file. This is created with default max tracks and events
//
//----------------------------------------------------------------------------
HRESULT WINAPI 
    D3DXLoadMeshHierarchyFromXA(
        LPCSTR Filename,
        DWORD MeshOptions,
        LPDIRECT3DDEVICE9 pD3DDevice,
        LPD3DXALLOCATEHIERARCHY pAlloc,
        LPD3DXLOADUSERDATA pUserDataLoader, 
        LPD3DXFRAME *ppFrameHierarchy,
        LPD3DXANIMATIONCONTROLLER *ppAnimController
);

HRESULT WINAPI 
    D3DXLoadMeshHierarchyFromXW(
        LPCWSTR Filename,
        DWORD MeshOptions,
        LPDIRECT3DDEVICE9 pD3DDevice,
        LPD3DXALLOCATEHIERARCHY pAlloc,
        LPD3DXLOADUSERDATA pUserDataLoader, 
        LPD3DXFRAME *ppFrameHierarchy,
        LPD3DXANIMATIONCONTROLLER *ppAnimController
);

#ifdef UNICODE
#define D3DXLoadMeshHierarchyFromX D3DXLoadMeshHierarchyFromXW
#else
#define D3DXLoadMeshHierarchyFromX D3DXLoadMeshHierarchyFromXA
#endif

HRESULT WINAPI 
    D3DXLoadMeshHierarchyFromXInMemory(
        LPCVOID Memory,
        DWORD SizeOfMemory,
        DWORD MeshOptions,
        LPDIRECT3DDEVICE9 pD3DDevice,
        LPD3DXALLOCATEHIERARCHY pAlloc,
        LPD3DXLOADUSERDATA pUserDataLoader, 
        LPD3DXFRAME *ppFrameHierarchy,
        LPD3DXANIMATIONCONTROLLER *ppAnimController
);

//----------------------------------------------------------------------------
// D3DXSaveMeshHierarchyToFile:
// ---------------------------
// Creates a .X file and saves the mesh hierarchy and corresponding animations
// in it
//
// Parameters:
//  Filename
//      Name of the .X file
//  XFormat
//      Format of the .X file (text or binary, compressed or not, etc)
//  pFrameRoot
//      Root node of the hierarchy to be saved
//  pAnimController
//      The animation mixer whose animation sets are to be stored
//  pUserDataSaver
//      Application provided interface to allow adding of user data to
//        data objects saved to .X file
//
//----------------------------------------------------------------------------
HRESULT WINAPI 
    D3DXSaveMeshHierarchyToFileA(
        LPCSTR Filename,
        DWORD XFormat,
        LPD3DXFRAME pFrameRoot, 
        LPD3DXANIMATIONCONTROLLER pAnimMixer,
        LPD3DXSAVEUSERDATA pUserDataSaver
);

HRESULT WINAPI 
    D3DXSaveMeshHierarchyToFileW(
        LPCWSTR Filename,
        DWORD XFormat,
        LPD3DXFRAME pFrameRoot, 
        LPD3DXANIMATIONCONTROLLER pAnimMixer,
        LPD3DXSAVEUSERDATA pUserDataSaver
);

#ifdef UNICODE
#define D3DXSaveMeshHierarchyToFile D3DXSaveMeshHierarchyToFileW
#else
#define D3DXSaveMeshHierarchyToFile D3DXSaveMeshHierarchyToFileA
#endif

//----------------------------------------------------------------------------
// D3DXFrameDestroy:
// -----------------
// Destroys the subtree of frames under the root, including the root
//
// Parameters:
//	pFrameRoot
//		Pointer to the root node
//  pAlloc
//      Allocation interface used to de-allocate nodes of the frame hierarchy
//
//----------------------------------------------------------------------------
HRESULT WINAPI
    D3DXFrameDestroy(
        LPD3DXFRAME pFrameRoot,
        LPD3DXALLOCATEHIERARCHY pAlloc
);

//----------------------------------------------------------------------------
// D3DXFrameAppendChild:
// ---------------------
// Add a child frame to a frame
//
// Parameters:
//	pFrameParent
//		Pointer to the parent node
//  pFrameChild
//      Pointer to the child node
//
//----------------------------------------------------------------------------
HRESULT WINAPI 
    D3DXFrameAppendChild(
        LPD3DXFRAME pFrameParent,
        LPD3DXFRAME pFrameChild
);

//----------------------------------------------------------------------------
// D3DXFrameFind:
// --------------
// Finds a frame with the given name.  Returns NULL if no frame found.
//
// Parameters:
//	pFrameRoot
//		Pointer to the root node
//  Name
//      Name of frame to find
//
//----------------------------------------------------------------------------
LPD3DXFRAME WINAPI 
    D3DXFrameFind(
        LPD3DXFRAME pFrameRoot,
        LPCSTR Name
);

//----------------------------------------------------------------------------
// D3DXFrameRegisterNamedMatrices:
// --------------------------
// Finds all frames that have non-null names and registers each of those frame
// matrices to the given animation mixer
//
// Parameters:
//	pFrameRoot
//		Pointer to the root node
//	pAnimMixer
//		Pointer to the animation mixer where the matrices are registered
//
//----------------------------------------------------------------------------
HRESULT WINAPI
    D3DXFrameRegisterNamedMatrices(
        LPD3DXFRAME pFrameRoot,
        LPD3DXANIMATIONCONTROLLER pAnimMixer
);

//----------------------------------------------------------------------------
// D3DXFrameNumNamedMatrices:
// --------------------------
// Counts number of frames in a subtree that have non-null names 
//
// Parameters:
//	pFrameRoot
//		Pointer to the root node of the subtree
// Return Value:
//		Count of frames
//
//----------------------------------------------------------------------------
UINT WINAPI
    D3DXFrameNumNamedMatrices(
        LPD3DXFRAME pFrameRoot
);

//----------------------------------------------------------------------------
// D3DXFrameCalculateBoundingSphere:
// ---------------------------------
// Computes the bounding sphere of all the meshes in the frame hierarchy
//
// Parameters:
//	pFrameRoot
//		Pointer to the root node
//	pObjectCenter
//		Returns the center of the bounding sphere
//	pObjectRadius
//		Returns the radius of the bounding sphere
//
//----------------------------------------------------------------------------
HRESULT WINAPI
    D3DXFrameCalculateBoundingSphere(
        LPD3DXFRAME pFrameRoot, 
        LPD3DXVECTOR3 pObjectCenter, 
        FLOAT *pObjectRadius
);

#ifdef __cplusplus
}
#endif //__cplusplus

#undef INTERFACE
#define INTERFACE ID3DXInterpolator

//----------------------------------------------------------------------------
// This interface defines a SRT (scale/rotate/translate) interpolator. This
// is an abstract interface. ID3DXKeyFrameInterpolator inherits from this.
// An application can implement this for custom SRT interpolator
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXInterpolator /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(ID3DXInterpolator, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXInterpolator
    STDMETHOD_(LPCSTR, GetName)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetPeriod)(THIS) PURE;

	//----------------------------------------------------------------------------
	// GetSRT:
	// -------
	// Returns the scale, rotation and translation at a given time
	//
	// Parameters:
	//	Time
	//		Time at which the interpolator should be queried
	//	pScale
	//		Returns the scale vector
	//	pRotate
	//		Returns the rotation qaternion
	//	pTranslate
	//		Returns the translate vector
	//
	//----------------------------------------------------------------------------
    STDMETHOD(GetSRT)(THIS_ DOUBLE Time, D3DXVECTOR3 *pScale, D3DXQUATERNION *pRotate, D3DXVECTOR3 *pTranslate) PURE;
    STDMETHOD(GetLastSRT)(THIS_ D3DXVECTOR3 *pScale, D3DXQUATERNION *pRotate, D3DXVECTOR3 *pTranslate) PURE;
};




//----------------------------------------------------------------------------
// This structure describes a vector key for use in keyframe animation.
// It specifies a vector Value at a given Time. This is used for scale and
// translation keys
//----------------------------------------------------------------------------
typedef struct _D3DXKEY_VECTOR3
{
    FLOAT Time;
    D3DXVECTOR3 Value;
} D3DXKEY_VECTOR3, *LPD3DXKEY_VECTOR3;


//----------------------------------------------------------------------------
// This structure describes a quaternion key for use in keyframe animation.
// It specifies a quaternion Value at a given Time. This is used for rotation
// keys
//----------------------------------------------------------------------------
typedef struct _D3DXKEY_QUATERNION
{
    FLOAT Time;
    D3DXQUATERNION Value;
} D3DXKEY_QUATERNION, *LPD3DXKEY_QUATERNION;


#undef INTERFACE
#define INTERFACE ID3DXKeyFrameInterpolator

//----------------------------------------------------------------------------
// This interface implements an SRT (scale/rotate/translate) interpolator
// It takes a scattered set of keys and interpolates the transform for any
// given time
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXKeyFrameInterpolator /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(ID3DXKeyFrameInterpolator, ID3DXInterpolator)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXInterpolator
    STDMETHOD_(LPCSTR, GetName)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetPeriod)(THIS) PURE;

    STDMETHOD(GetSRT)(THIS_ DOUBLE Time, D3DXVECTOR3 *pScale, D3DXQUATERNION *pRotate, D3DXVECTOR3 *pTranslate) PURE;
    STDMETHOD(GetLastSRT)(THIS_ D3DXVECTOR3 *pScale, D3DXQUATERNION *pRotate, D3DXVECTOR3 *pTranslate) PURE;

    // ID3DXKeyFrameInterpolator
    STDMETHOD_(UINT, GetNumScaleKeys)(THIS) PURE;
    STDMETHOD(GetScaleKeys)(THIS_ LPD3DXKEY_VECTOR3 pKeys) PURE;

    STDMETHOD_(UINT, GetNumRotationKeys)(THIS) PURE;
    STDMETHOD(GetRotationKeys)(THIS_ LPD3DXKEY_QUATERNION pKeys) PURE;

    STDMETHOD_(UINT, GetNumTranslationKeys)(THIS) PURE;
    STDMETHOD(GetTranslationKeys)(THIS_ LPD3DXKEY_VECTOR3 pKeys) PURE;

    // the value passed to D3DXCreateKeyFrameInterpolator to scale from the times in LPD3DXKEY_VECTOR3 to global/anim time.
    STDMETHOD_(DOUBLE, GetSourceTicksPerSecond)(THIS) PURE;
};



//----------------------------------------------------------------------------
// This interface implements an set of interpolators. The set consists of
// interpolators for many nodes for the same animation. 
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// ID3DXAnimationSet /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(ID3DXAnimationSet, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXAnimationSet
    STDMETHOD_(LPCSTR, GetName)(THIS) PURE;
    STDMETHOD_(DOUBLE, GetPeriod)(THIS) PURE;

    STDMETHOD_(UINT, GetNumInterpolators)(THIS) PURE;
    STDMETHOD(GetInterpolatorByIndex)(THIS_ UINT Index, LPD3DXINTERPOLATOR *ppInterpolator) PURE;
    STDMETHOD(GetInterpolatorByName)(THIS_ LPCSTR pName, LPD3DXINTERPOLATOR *ppInterpolator) PURE;
};

//----------------------------------------------------------------------------
// This structure describes an animation track. A track is a combination
//  of an animation set (stored separately) and mixing information.
//  the mixing information consists of the current position, speed, and blending
//  weight for the track.  The Flags field also specifies whether the track
//  is low or high priority.  Tracks with the same priority are blended together
//  and then the two resulting values are blended using the priority blend factor.
//----------------------------------------------------------------------------
typedef struct _D3DXTRACK_DESC
{
    DWORD Flags;
    FLOAT Weight;
    FLOAT Speed;
    BOOL  Enable;
    DOUBLE AnimTime;
} D3DXTRACK_DESC, *LPD3DXTRACK_DESC;

//----------------------------------------------------------------------------
// This enum defines the type of transtion performed on a event that transitions from one value to another
//----------------------------------------------------------------------------
typedef enum _D3DXTRACKFLAG {
    D3DXTF_LOWPRIORITY            = 0x000,     // This track should be blended with all low priority tracks before mixed with the high priority result
    D3DXTF_HIGHPRIORITY           = 0x001,     // This track should be blended with all high priority tracks before mixed with the low priority result

    D3DXTF_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DXTRACKFLAG;


//----------------------------------------------------------------------------
// This interface implements the main animation functionality. It connects
// animation sets with the transform frames that are being animated. Allows
// mixing multiple animations for blended animations or for transistions
// It adds also has methods to modify blending parameters over time to 
// enable smooth transistions and other effects.
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// This enum defines the type of transtion performed on a event that transitions from one value to another
//----------------------------------------------------------------------------
typedef enum _D3DXTRANSITIONTYPE {
    D3DXTRANSITION_LINEAR            = 0x000,     // Linear transition from one value to the next
    D3DXTRANSITION_EASEINEASEOUT     = 0x001,     // Ease-In Ease-Out spline transtion from one value to the next

    D3DXTRANSITION_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DXTRANSITIONTYPE;


//////////////////////////////////////////////////////////////////////////////
// ID3DXAnimationController //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(ID3DXAnimationController, IUnknown)
{
  // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

  // mixing functionality
    // register outputs of SetTime
    STDMETHOD(RegisterMatrix)(THIS_ LPCSTR Name, D3DXMATRIX *pMatrix) PURE;


    // AnimationSets
    STDMETHOD_(UINT, GetNumAnimationSets)(THIS) PURE;
    STDMETHOD(GetAnimationSet)(THIS_ DWORD iAnimationSet, LPD3DXANIMATIONSET *ppAnimSet) PURE;

    STDMETHOD(RegisterAnimationSet)(THIS_ LPD3DXANIMATIONSET pAnimSet) PURE;
    STDMETHOD(UnregisterAnimationSet)(THIS_ LPD3DXANIMATIONSET pAnimSet) PURE;


    // Tracks
    STDMETHOD_(UINT, GetMaxNumTracks)(THIS) PURE;
    STDMETHOD(GetTrackDesc)(THIS_ DWORD Track, D3DXTRACK_DESC *pDesc) PURE;
    STDMETHOD(SetTrackDesc)(THIS_ DWORD Track, D3DXTRACK_DESC *pDesc) PURE;
    
    STDMETHOD(GetTrackAnimationSet)(THIS_ DWORD Track, LPD3DXANIMATIONSET *ppAnimSet) PURE;
    STDMETHOD(SetTrackAnimationSet)(THIS_ DWORD Track, LPD3DXANIMATIONSET pAnimSet) PURE;

    // Individual track field access
    STDMETHOD(SetTrackSpeed)(THIS_ DWORD Track, FLOAT Speed) PURE;
    STDMETHOD(SetTrackWeight)(THIS_ DWORD Track, FLOAT Weight) PURE;
    STDMETHOD(SetTrackAnimTime)(THIS_ DWORD Track, DOUBLE AnimTime) PURE;
    STDMETHOD(SetTrackEnable)(THIS_ DWORD Track, BOOL Enable) PURE;

    // Time
    STDMETHOD_(DOUBLE, GetTime)(THIS) PURE;
    STDMETHOD(SetTime)(THIS_ DOUBLE Time) PURE;

    STDMETHOD(CloneAnimationController)(THIS_ UINT MaxNumMatrices, UINT MaxNumAnimationSets, UINT MaxNumTracks, UINT MaxNumEvents, LPD3DXANIMATIONCONTROLLER *ppAnimController) PURE;

    STDMETHOD_(UINT, GetMaxNumMatrices)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumEvents)(THIS) PURE;
    STDMETHOD_(UINT, GetMaxNumAnimationSets)(THIS) PURE;

  // Sequencing abilities
    STDMETHOD(KeyTrackSpeed)(THIS_ DWORD Track, FLOAT NewSpeed, DOUBLE StartTime, DOUBLE Duration, DWORD Method) PURE;
    STDMETHOD(KeyTrackWeight)(THIS_ DWORD Track, FLOAT NewWeight, DOUBLE StartTime, DOUBLE Duration, DWORD Method) PURE;
    STDMETHOD(KeyTrackAnimTime)(THIS_ DWORD Track, DOUBLE NewAnimTime, DOUBLE StartTime) PURE;
    STDMETHOD(KeyTrackEnable)(THIS_ DWORD Track, BOOL NewEnable, DOUBLE StartTime) PURE;

    // this functions sets the blend weight to be used to blend high and low priority tracks together.
    //  NOTE: this has no effect unless there are active animations on tracks for a given matrix that have both high and low results
    STDMETHOD_(FLOAT, GetPriorityBlend)(THIS) PURE;
    STDMETHOD(SetPriorityBlend)(THIS_ FLOAT BlendWeight) PURE;

    STDMETHOD(KeyPriorityBlend)(THIS_ FLOAT NewBlendWeight, DOUBLE StartTime, DOUBLE Duration, DWORD Method) PURE;
};

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//----------------------------------------------------------------------------
// D3DXCreateKeyFrameInterpolator:
// -------------------------------
// Creates a SRT key frame interpolator object from the given set of keys
//
// Parameters:
//	ScaleKeys
//		Array of scale key vectors
//	NumScaleKeys
//		Num elements in ScaleKeys array
//	RotationKeys
//		Array of rotation key quternions
//	NumRotationKeys
//		Num elements in RotationKeys array
//	TranslateKeys
//		Array of translation key vectors
//	NumTranslateKeys
//		Num elements in TranslateKeys array
//	ScaleInputTimeBy
//		All key times are scaled by this factor
//	ppNewInterpolator
//		Returns the keyframe interpolator interface
//
//----------------------------------------------------------------------------
HRESULT WINAPI
D3DXCreateKeyFrameInterpolator(LPCSTR Name, 
                            LPD3DXKEY_VECTOR3    ScaleKeys,     UINT NumScaleKeys,
                            LPD3DXKEY_QUATERNION RotationKeys,  UINT NumRotationKeys,
                            LPD3DXKEY_VECTOR3    TranslateKeys, UINT NumTranslateKeys,
                            DOUBLE ScaleInputTimeBy, LPD3DXKEYFRAMEINTERPOLATOR *ppNewInterpolator);

//----------------------------------------------------------------------------
// D3DXCreateAnimationSet:
// -----------------------
// Creates an animtions set interface given a set of interpolators
//
// Parameters:
//	Name
//		Name of the animation set
//	pInterpolators
//		Array of interpolators
//	NumInterpolators
//		Num elements in the pInterpolators array
//	ppAnimSet
//		Returns the animation set interface
//
//-----------------------------------------------------------------------------		
HRESULT WINAPI
D3DXCreateAnimationSet(LPCSTR Name, 
                            LPD3DXINTERPOLATOR *ppInterpolators, UINT NumInterpolators,
                            LPD3DXANIMATIONSET *ppAnimSet);

//----------------------------------------------------------------------------
// D3DXCreateAnimationController:
// -------------------------
// Creates an animtion mixer object
//
// Parameters:
//	MaxNumMatrices
//		The upper limit for the number of matrices that can be animated by the
//		the object
//	MaxNumAnimationSets
//		The upper limit of the number of animation sets that can be played by
//		the object
//	MaxNumTracks
//		The upper limit of the number of animation sets that can be blended at
//		any time.
//	MaxNumEvents
//		The upper limit of the number of outstanding events that can be
//		scheduled at once.
//	ppAnimController
//		Returns the animation controller interface
//
//-----------------------------------------------------------------------------		
HRESULT WINAPI
D3DXCreateAnimationController(UINT MaxNumMatrices, UINT MaxNumAnimationSets, UINT MaxNumTracks, UINT MaxNumEvents,
                                 LPD3DXANIMATIONCONTROLLER *ppAnimController);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__D3DX9ANIM_H__


