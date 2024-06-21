# Generating import libraries (.lib files)

Usually when linking with a certain dll, you’d use an import library provided by the vendor. 
In our case, no such ci.lib file is provided and we need to generate it ourselves.
This lib file should be added as a linker input in the project properties.

## 64 bit

Get the exported functions from the dll, using dumpbin utility: 

`dumpbin /EXPORTS c:\windows\system32\ci.dll`

Create a .def file. It will looks something like this:

```c
LIBRARY ci.dll
EXPORTS
    CiValidateFileAsImageType       @1  NONAME
    CiRegisterSigningInformation    @2  NONAME
    CiUnregisterSigningInformation  @3  NONAME
    CiCheckSignedFile
    CiFindPageHashesInCatalog
    CiFindPageHashesInSignedFile
    CiFreePolicyInfo
    CiGetCertPublisherName
    CiGetPEInformation
    CiInitialize
    CiSetTrustedOriginClaimId
    CiValidateFileObject
    CiVerifyHashInCatalog
```

Generate the .lib file using the lib utility:

`lib /def:ci.def /machine:x64 /out:ci.lib`


## 32 bit

Here the situation gets a bit trickier, since in 32bit the functions are decorated to
include the sum of the arguments (in bytes), for example:

`CiFreePolicyInfo@4`

But ci.dll is exporting the functions in their non-decorated shape, so we need to create a .lib file that makes this translation.

- Follow the first two steps of the 64bit section above.

- Create a C++ file with function stubs - the same signature but dummy body. You basically mimic what the vendor did when exporting
  the functions from their code. For example:

```c
_IRQL_requires_max_(PASSIVE_LEVEL)
PVOID
NTAPI
CiFreePolicyInfo(
    _In_ MINCRYPT_POLICY_INFO* PolicyInfo
)
{
    UNREFERENCED_PARAMETER(PolicyInfo);
    return nullptr;
}
```

An example of such file is included in this repo under the name Stub.Ci.cpp

- Compile it into an OBJ file.

```bat

> SET KM_IncludePath="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\km"
> SET CRT_IncludePath="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\km\crt"
> SET KIT_SHARED_IncludePath="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\shared"
>
> cl Stub.Ci.cpp /c /kernel /Zc:wchar_t /I%KM_IncludePath% /I%CRT_IncludePath% /I%KIT_SHARED_IncludePath% /D _X86_=1 /D i386=1 /DSTD_CALL /D_MINCRYPT_LIB
```

- Generate the .lib file using the lib utility, this time with the OBJ file:

```bat
> lib /def:ci.def /machine:x86 /out:ci.lib Stub.Ci.obj
```
